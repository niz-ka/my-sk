#include <cstdlib>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <random>
#include "Server.h"
#include "Message.h"
#include "helpers.h"

#define HEADER 4

Server::Server(int _port) : socketFd(0), address({}), clients({}), pollfds({}), games({}), port(_port) {}

size_t Server::getNumberOfClients() const {
    return this->clients.size();
}

void Server::disconnectClient(int clientFd, unsigned long &i) {
    printf("[INFO] Client with address %s and port %d disconnected. Number of clients: %zu\n",
           inet_ntoa(this->clients[clientFd].getAddressPointer()->sin_addr),
           this->clients[clientFd].getAddressPointer()->sin_port,
           this->getNumberOfClients() - 1);

    const int gameCode = clients[clientFd].getGameCode();

    // To właściciel jakiejś gry
    if (games[gameCode].getOwnerSocket() == clientFd) {
        for (auto &client: clients) {
            if (client.first == clientFd || client.second.getGameCode() != gameCode) continue;
            sendData(client.first, msgToStr(MESSAGE::CONNECTION_LOST));
            client.second.setGameCode(0);
            client.second.setNick("");
            client.second.setAnswered(false);
        }
        games.erase(gameCode);
    }

    // Usuń *tego* klienta
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);

    this->clients.erase(clientFd);

    pollfds.erase(pollfds.begin() + (long) i);
    --i;
}

int Server::connectClient() {
    // Akceptuj nowego klienta
    sockaddr_in remoteAddress{};
    socklen_t size = sizeof(remoteAddress);
    int clientSocket = accept(this->socketFd, (sockaddr *) &remoteAddress, &size);

    // Błąd, pomiń klienta
    if (clientSocket == -1) {
        perror("[ERROR] accept()");
        return -1;
    }

    // Dodaj gniazdo klienckie do obserwowanych
    this->clients[clientSocket] = Client(remoteAddress);
    printf("[INFO] %zu. client connected with ip: %s and port: %d\n",
           this->getNumberOfClients(),
           inet_ntoa(this->clients[clientSocket].getAddressPointer()->sin_addr),
           this->clients[clientSocket].getAddressPointer()->sin_port);

    return clientSocket;
}

void Server::terminate(const std::string &description) {
    perror(("[ERROR] " + description).c_str());

    // Zamknij gniazda klientów
    for (auto &client: this->clients) {
        shutdown(client.first, SHUT_RDWR);
        close(client.first);
    }

    this->clients.clear();

    // Zamknij gniazdo nasłuchujące serwera
    shutdown(this->socketFd, SHUT_RDWR);
    close(this->socketFd);

    exit(EXIT_FAILURE);
}


void Server::makeAction(std::string &message, const int clientFd) {

    const MESSAGE action = extractMsg(message);

    if (action == MESSAGE::QUESTION) {
        const MESSAGE prefix = extractMsg(message);

        // Stwórz nową grę (rozpoczęto wysyłanie pytań)
        if (prefix == MESSAGE::QUESTION_START) {

            for (auto &game: games) {
                if (game.second.getOwnerSocket() == clientFd || clients[clientFd].getGameCode() != 0) {
                    printf("[ERROR] Permission denied\n");
                    return;
                }
            }

            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist6(1000, 9999);
            int code = static_cast<int>(dist6(rng));

            // Gra o wylosowanym kluczu istnieje!
            while (games.find(code) != games.end()) {
                code = static_cast<int>(dist6(rng));
            }

            // Stwórz grę o podanym kodzie i deskryptorem właściciela
            this->games[code] = Game(clientFd);
            // Przypisz temu klientowi grę o tym kodzie
            this->clients[clientFd].setGameCode(code);

            printf("[INFO] Game with socket (%d) and code (%d) created\n", clientFd, code);
            return;
        }

        if (clientFd != games[clients[clientFd].getGameCode()].getOwnerSocket() ||
            games[clients[clientFd].getGameCode()].isStarted()) {
            printf("[INFO] Permission denied\n");
            return;
        }

        // Zakończono wysyłanie pytań, wyślij kod gry
        if (prefix == MESSAGE::QUESTION_END) {
            sendData(clientFd, std::to_string(clients[clientFd].getGameCode()));
            printf("[INFO] Game code (%d) sent to (%d)\n", clients[clientFd].getGameCode(), clientFd);
        } else if (prefix == MESSAGE::AUTO_NEXT) {
            bool autoNextQuestion = (message.substr(0, 1) == "y");
            games[clients[clientFd].getGameCode()].setAutoNextQuestion(autoNextQuestion);
        }

        // Wysłano część jednego z pytań
        else {
            const int gameCode = clients[clientFd].getGameCode();

            // Otrzymano pytanie pytania
            if (prefix == MESSAGE::QUESTION_QUESTION) {
                this->games[gameCode].getQuestions().emplace_back();
                this->games[gameCode].getQuestions().back().setQuestion(message);
            } else if (prefix == MESSAGE::QUESTION_ANSWER_A) {
                this->games[gameCode].getQuestions().back().setAnswerA(message);
            } else if (prefix == MESSAGE::QUESTION_ANSWER_B) {
                this->games[gameCode].getQuestions().back().setAnswerB(message);
            } else if (prefix == MESSAGE::QUESTION_ANSWER_C) {
                this->games[gameCode].getQuestions().back().setAnswerC(message);
            } else if (prefix == MESSAGE::QUESTION_ANSWER_D) {
                this->games[gameCode].getQuestions().back().setAnswerD(message);
            } else if (prefix == MESSAGE::QUESTION_CORRECT) {
                this->games[gameCode].getQuestions().back().setCorrect(message);
            } else if (prefix == MESSAGE::QUESTION_TIME) {
                this->games[gameCode].getQuestions().back().setTime(message);
            } else {
                printf("[ERROR] Unknown question part type!\n");
                return;
            }
        }
    }

    // Ktoś próbuje dołączyć do gry (wpisał kod)
    else if (action == MESSAGE::JOINING) {
        const int submittedCode = stringToInt(message);

        if (clients[clientFd].getGameCode() != 0) {
            printf("[ERROR] Permission denied\n");
            return;
        }

        for (auto &game: this->games) {

            const int correctCode = game.first;

            // Znaleziono grę o podanym kodzie
            if (submittedCode == correctCode) {

                // Gra już trwa
                if (game.second.isStarted()) {
                    sendData(clientFd, msgToStr(MESSAGE::GAME_ALREADY_STARTED));
                    printf("[INFO] Game is already running (gameCodeView)\n");
                    return;
                }

                // Gracz dołącza do gry
                sendData(clientFd, msgToStr(MESSAGE::JOINING_OK));
                this->clients[clientFd].setGameCode(correctCode);
                printf("[INFO] Player (%d) typed correct code (%d)\n", clientFd, correctCode);
                return;
            }
        }

        sendData(clientFd, msgToStr(MESSAGE::GAME_NOT_EXISTS));
        printf("[INFO] Game with code (%d) not exists\n", submittedCode);
    }

    // Gracz wysłał pseudonim
    else if (action == MESSAGE::NICK_CHOOSING) {
        const std::string &nick = message;
        const int gameCode = clients[clientFd].getGameCode();

        if (clientFd == games[gameCode].getOwnerSocket() || gameCode == 0) {
            printf("[ERROR] Permission denied\n");
            return;
        }

        if (games.find(gameCode) == games.end()) {
            printf("[INFO] Game with code (%d) not exists\n", gameCode);
            sendData(clientFd, msgToStr(MESSAGE::GAME_NOT_EXISTS));
        }


        // Sprawdź, czy gra już nie trwa!
        if (this->games[gameCode].isStarted()) {
            // Usuwamy gracza z gry
            this->clients[clientFd].setGameCode(0);
            sendData(clientFd, msgToStr(MESSAGE::GAME_ALREADY_STARTED));
            printf("[INFO] Game with code (%d) is already running (gameNickView)\n", gameCode);
            return;
        }

        // Sprawdź unikalność pseudonimu w obrębie gry
        for (auto &anotherClient: this->clients) {
            const int anotherClientFd = anotherClient.first;

            // Nie porównuj się ze sobą
            if (clientFd == anotherClientFd) continue;

            if ((nick == anotherClient.second.getNick()) && (gameCode == anotherClient.second.getGameCode())) {
                sendData(clientFd, msgToStr(MESSAGE::NICK_USED));
                printf("[INFO] Nick (%s) already exists for game with code (%d)\n", nick.c_str(), gameCode);
                return;
            }
        }

        this->clients[clientFd].setNick(nick);

        // Wyślij wiadomość do twórcy gry o nowym graczu
        const int ownerSocket = games[gameCode].getOwnerSocket();
        sendData(ownerSocket, msgToStr(MESSAGE::NEW_PLAYER) + nick);

        sendData(clientFd, msgToStr(MESSAGE::NICK_OK));
        printf("[INFO] Nick (%s) for player (%d) set\n", nick.c_str(), clientFd);
    }

    // Właściciel gry wysłał sygnał do rozpoczęcia gry
    else if (action == MESSAGE::GAME_START) {
        const int gameCode = clients[clientFd].getGameCode();
        // Ktoś nie jest właścicielem, a próbuje rozpocząć grę!
        if (games[gameCode].getOwnerSocket() != clientFd) {
            printf("[INFO] Permission denied!\n");
            return;
        }

        if (games[gameCode].isStarted()) {
            printf("[INFO] Permission denied!\n");
            return;
        }

        this->games[gameCode].setStarted(true);

        std::string autoNextQuestion = games[gameCode].isAutoNextQuestion() ? "y" : "n";

        sendData(clientFd, msgToStr(MESSAGE::AUTO_NEXT) + autoNextQuestion);

        // Wyślij sygnał start
        for (auto &client: clients) {
            if (client.first == clientFd) continue;

            if (client.second.getGameCode() == gameCode) {
                sendData(client.first, msgToStr(MESSAGE::GAME_START));
            }
        }

        for (auto &client: clients) {
            if (client.first == clientFd || gameCode != client.second.getGameCode()) continue;
            for (auto &anotherClient: clients) {
                if (anotherClient.first == clientFd || gameCode != anotherClient.second.getGameCode() ||
                    anotherClient.first == client.first)
                    continue;
                sendData(client.first, msgToStr(MESSAGE::NEW_PLAYER) + anotherClient.second.getNick());
            }
        }

        // Wyślij pierwsze pytanie
        for (auto &client: clients) {
            if (client.second.getGameCode() == gameCode) {
                Question &question = games[gameCode].getQuestions()[0];
                sendData(client.first, msgToStr(MESSAGE::QUESTION));
                sendData(client.first, question.getQuestion());
                sendData(client.first, question.getAnswerA());
                sendData(client.first, question.getAnswerB());
                sendData(client.first, question.getAnswerC());
                sendData(client.first, question.getAnswerD());
                sendData(client.first, question.getTime());
                sendData(client.first, question.getCorrect());
            }
        }

        printf("[INFO] Game owner started game with code (%d)\n", gameCode);

    } else if (action == MESSAGE::ANSWER) {
        const int submittedQuestionNumber = std::stoi(message.substr(0, 3));
        const std::string submittedAnswer = message.substr(3, 1);
        const int gameCode = clients[clientFd].getGameCode();
        const int gameOwner = games[gameCode].getOwnerSocket();

        if (submittedAnswer != "a" && submittedAnswer != "b" && submittedAnswer != "c" && submittedAnswer != "d") {
            printf("[ERROR] Wrong answer type\n");
            return;
        }

        if (!games[gameCode].isStarted()) {
            printf("[ERROR] Game with code (%d) is not running\n", gameCode);
            return;
        }

        // Właściciel nie może odpowiadać
        if (clientFd == games[gameCode].getOwnerSocket()) {
            printf("[ERROR] Permission denied\n");
            return;
        }

        // Użytkownik już odpowiedział na to pytanie!
        if (clients[clientFd].isAnswered()) {
            printf("[INFO] Permission denied\n");
            return;
        }

        // Odpowiedź spóźniona
        if (submittedQuestionNumber != games[gameCode].getCurrentQuestion()) {
            printf("[INFO] Player (%s) sent answer for question (%d) in game (%d) too late\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
            return;
        }

        std::string point;

        // Odpowiedź poprawna
        if (games[gameCode].getQuestions()[submittedQuestionNumber].getCorrect() == submittedAnswer) {
            point = "1";
            printf("[INFO] Player (%s) sent correct answer for question (%d) in game (%d)\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
        } else {
            point = "0";
            printf("[INFO] Player (%s) sent incorrect answer for question (%d) in game (%d)\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
        }

        for (auto &client: clients) {
            if (client.second.getGameCode() != gameCode) continue;
            sendData(client.first, msgToStr(MESSAGE::PLAYERS_RANK) + point + clients[clientFd].getNick());
        }

        sendData(gameOwner,
                 msgToStr(MESSAGE::OWNER_RANK) + message.substr(0, 3) + submittedAnswer + clients[clientFd].getNick());

    } else if (action == MESSAGE::NEXT_QUESTION) {
        const int gameCode = clients[clientFd].getGameCode();
        const int questionNumber = std::stoi(message.substr(0, 3));

        // Ktoś nie jest właścicielem, a próbuje dać następne pytanie!
        if (games[gameCode].getOwnerSocket() != clientFd) {
            printf("[INFO] Permission denied!\n");
            return;
        }

        // Gra nie trwa
        if (!games[gameCode].isStarted()) {
            printf("[ERROR] Game with code (%d) is not running\n", gameCode);
            return;
        }

        // Spóźnione
        if (questionNumber != games[gameCode].getCurrentQuestion()) {
            printf("[INFO] Next question in game (%d) too late\n", gameCode);
            return;
        }

        // To już było ostatnie pytanie
        if ((questionNumber == static_cast<int>(games[gameCode].getQuestions().size() - 1))) {
            for (auto &client: clients) {
                if (client.second.getGameCode() == gameCode) {
                    sendData(client.first, msgToStr(MESSAGE::GAME_END));
                    client.second.setGameCode(0);
                    client.second.setAnswered(false);
                    client.second.setNick("");
                }
            }
            games.erase(gameCode);
            return;
        }

        const int number = games[gameCode].nextQuestion();
        const int questionsSize = static_cast<int>(games[gameCode].getQuestions().size());

        if (number >= questionsSize) {
            printf("[ERROR] Question number out of range\n");
            return;
        }

        for (auto &client: clients) {
            if (client.second.getGameCode() == gameCode) {
                const Question &question = games[gameCode].getQuestions()[number];

                sendData(client.first, msgToStr(MESSAGE::QUESTION));
                sendData(client.first, question.getQuestion());
                sendData(client.first, question.getAnswerA());
                sendData(client.first, question.getAnswerB());
                sendData(client.first, question.getAnswerC());
                sendData(client.first, question.getAnswerD());
                sendData(client.first, question.getTime());
                sendData(client.first, question.getCorrect());

                client.second.setAnswered(false);
            }
        }

        printf("[INFO] Next question (%d) sent in game (%d)\n", number, gameCode);
    } else {
        printf("[ERROR] Unknown message (%s)\n", msgToStr(action).c_str());
    }

}

void Server::run() {

    this->socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (this->socketFd == -1) {
        perror("[ERROR] socket()");
        exit(EXIT_FAILURE);
    }

    // Umożliwiaj ponownie bindowanie od razu
    const int opt = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        this->terminate("setsockopt()");
    }

    this->address.sin_addr.s_addr = htonl(INADDR_ANY);
    this->address.sin_port = htons(this->port);
    this->address.sin_family = AF_INET;

    if (bind(this->socketFd, (sockaddr *) &this->address, sizeof(sockaddr_in)) == -1) {
        this->terminate("bind()");
    }

    if (listen(this->socketFd, 32) == -1) {
        this->terminate("listen()");
    }

    printf("[INFO] Server is listening on ADDRESS: %s | PORT: %d\n",
           inet_ntoa(this->address.sin_addr),
           htons(this->address.sin_port));

    pollfds.push_back({this->socketFd, POLLIN, 0});

    // Główna pętla programu
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        int error_flag = poll(pollfds.data(), pollfds.size(), -1);

        // Błąd poll!
        if (error_flag == -1) {
            this->terminate("poll()");
        }

        auto pollfds_size = pollfds.size();
        for (std::vector<pollfd>::size_type i = 0; i < pollfds_size; ++i) {

            // Nic wartego uwagi się nie wydarzyło
            if (pollfds[i].revents == 0) continue;

            // Wszystko inne niż POLLIN to błąd!
            if (pollfds[i].revents != POLLIN) {
                this->disconnectClient(pollfds[i].fd, i);
                pollfds_size = pollfds.size();
                printf("[ERROR] revents != POLLIN\n");
                continue;
            }

            // Zgłoszenie na gnieździe nasłuchującym
            if (pollfds[i].fd == this->socketFd) {
                int clientSocket = this->connectClient();
                if (clientSocket != -1) {
                    this->pollfds.push_back(pollfd{clientSocket, POLLIN, 0});
                    pollfds_size = pollfds.size();
                }
            } else {
                int clientFd = pollfds[i].fd;

                // Odbierz długość wiadomości
                std::string messageLength;
                int bytes = readData(clientFd, HEADER, messageLength);
                if (bytes == 0) {
                    this->disconnectClient(clientFd, i);
                    pollfds_size = pollfds.size();
                }

                if (bytes == 0 || bytes == -1) continue;

                // Konwersja string -> int
                int length = stringToInt(messageLength);
                if (length == -1) continue;

                // Odbierz wiadomość o długości przesłanej w nagłówku
                std::string message;
                bytes = readData(clientFd, length, message);
                if (bytes == 0) {
                    this->disconnectClient(clientFd, i);
                    pollfds_size = pollfds.size();
                }

                if (bytes == 0 || bytes == -1) continue;

                // Podejmij akcję w zależności od wiadomości
                this->makeAction(message, clientFd);
            }


        }
    }
    #pragma clang diagnostic pop
}