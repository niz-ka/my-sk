//
// Created by kamil on 08.01.2022.
//

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <random>
#include "Server.h"
#include "Message.h"

#define SERVER_PORT 5050
#define HEADER 4

Server::Server() : socketFd(0), address({}), clients({}), pollfds({}), games({}) {}

size_t Server::getNumberOfClients() const {
    return this->clients.size();
}


/**
 * Zamyka gniazdo i usuwa klienta z listy klientów
 * @param clientFd deskryptor gniazda klienta
 */
void Server::disconnectClient(int clientFd) {
    printf("[INFO] Client with address %s and port %d disconnected. Number of clients: %zu\n",
           inet_ntoa(this->clients[clientFd].getAddressPointer()->sin_addr),
           this->clients[clientFd].getAddressPointer()->sin_port,
           this->getNumberOfClients()-1);


//    // Usuń grę tego gracza (jeśli istnieje)
//    if (this->games.find(clientFd) != this->games.end()) {
//        std::vector<int> toRemove = {};
//
//        for(auto& client : this->clients) {
//            if((client.first != clientFd) &&
//            client.second.getGameOwnerSocket() == clientFd) {
//                shutdown(client.first, SHUT_RDWR);
//                close(client.first);
//                toRemove.push_back(client.first);
//            }
//        }
//
//        for(auto& element : toRemove) {
//            this->clients.erase(element);
//        }
//
//    }

    // Usuń dane tego klienta
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);

    // Ustaw właściciela gry na brak właściciela
    const int gameCode = clients[clientFd].getGameCode();
    if(games.find(gameCode) != games.end()) {
        if(games[gameCode].getOwnerSocket() == clientFd)
            games[gameCode].setOwnerSocket(0);
    }

    this->clients.erase(clientFd);
}

/**
 * Akceptuje nowego klienta i dodaje do listy klientów
 * @return deskryptor klienta lub -1 w przypadku błędu
 */
int Server::connectClient() {
    // Akceptuj nowego klienta
    sockaddr_in remoteAddress{};
    socklen_t size = sizeof(remoteAddress);
    int clientSocket = accept(this->socketFd, (sockaddr*)&remoteAddress, &size);

    // Błąd, pomiń klienta
    if(clientSocket == -1) {
        perror("[ERROR] accept()");
        return -1;
    }

    // Dodaj gniazdo klienckie do obserwowanych
    this->clients[clientSocket] = Client(clientSocket, remoteAddress);
    printf("[INFO] %zu. client connected with ip: %s and port: %d\n",
           this->getNumberOfClients(),
           inet_ntoa(this->clients[clientSocket].getAddressPointer()->sin_addr),
           this->clients[clientSocket].getAddressPointer()->sin_port);

    return clientSocket;
}

/**
 * Kończy działanie serwera z błędem
 * @param description Opis błędu (np. funkcja powodująca błąd)
 */
void Server::terminate(const std::string& description) {
    perror(("[ERROR] " + description).c_str());

    // Zamknij gniazda klientów
    for(auto& client : this->clients) {
        shutdown(client.first, SHUT_RDWR);
        close(client.first);
    }

    this->clients.clear();

    // Zamknij gniazdo nasłuchujące serwera
    shutdown(this->socketFd, SHUT_RDWR);
    close(this->socketFd);

    exit(EXIT_FAILURE);
}

/**
 * Czyta dane z zadanego gniazda
 * @param clientFd deskryptor gniazda klienta
 * @param length wielkość danych do odczytania
 * @param data referencja do zmiennej, w której zapisana zostanie wiadomość
 * @return liczbę przeczytanych bajtów / 0 gdy klient się rozłączył / -1 gdy wystąpił błąd
 */
size_t Server::readData(int clientFd, int length, std::string& data) {
    char* message = new char[length+1];
    size_t bytes;
    size_t bytesRead = 0;

    while(bytesRead < length)
    {
        bytes = recv(clientFd, message + bytesRead, length - bytesRead, 0);

        if(bytes == -1) {
            delete[] message;
            perror("[ERROR] recv()");
            return -1;
        }

        if(bytes == 0) {
            delete[] message;
            return 0;
        }

        bytesRead += bytes;
    }
    message[length] = '\0';
    data = std::string(message);
    delete[] message;

    return bytesRead;
}

/**
 * Konwertuje liczbę w postaci stringa do inta
 * @param number Liczba w postaci tekstowej
 * @return Liczbę w postaci liczbowej (-1 w przypadku błędu, TODO niezbyt trafnie jak z atoi :)
 */
int Server::stringToInt(const std::string& number) {

    int numberInt;

    try {
        numberInt = std::stoi(number);
    } catch (const std::invalid_argument& ia) {
        printf("[ERROR] Conversion impossible (%s)\n", number.c_str());
        return -1;
    } catch(const std::out_of_range& ofr) {
        perror("[ERROR] Conversion out ouf range");
        return -1;
    }

    return numberInt;

}

size_t Server::sendData(int socket, const std::string& data)
{
    const char* tmp = data.c_str();

    char* buf = new char[strlen(tmp) + 4];
    if( sprintf(buf, "%04zu%s", strlen(tmp), tmp) < 0) {
        perror("[ERROR] sprintf()");
        delete [] buf;
        return -1;
    }

    const size_t length = strlen(buf);

    size_t total = 0;
    size_t bytes;
    size_t bytesLeft = length;

    while(total < length) {
        bytes = send(socket, buf + total, bytesLeft, 0 );
        if(bytes == -1) {
            perror("[ERROR] send()");
            delete [] buf;
            return -1;
        }

        total += bytes;
        bytesLeft -= bytes;
    }

    delete [] buf;
    return total;
}

/**
 * Podejmuje akcję w zależności od otrzymanej wiadomości
 * @param message wiadomość przesłana przez klienta
 */
void Server::makeAction(std::string& message, const int clientFd) {

    const MESSAGE action = extractMsg(message);

    if(action == MESSAGE::QUESTION) {
       const MESSAGE prefix = extractMsg(message);

       // Stwórz nową grę (rozpoczęto wysyłanie pytań)
       if(prefix == MESSAGE::QUESTION_START) {
           std::random_device dev;
           std::mt19937 rng(dev());
           std::uniform_int_distribution<std::mt19937::result_type> dist6(1000,9999);
           int code = static_cast<int>(dist6(rng));

           // Gra o wylosowanym kluczu istnieje!
           while(games.find(code) != games.end()) {
               code = static_cast<int>(dist6(rng));
           }

           // Stwórz grę o podanym kodzie i deskryptorem właściciela
           this->games[code] = Game(clientFd, code);
           // Przypisz temu klientowi grę o tym kodzie
           this->clients[clientFd].setGameCode(code);

           printf("[INFO] Game with socket (%d) and code (%d) created\n", clientFd, code);
        }

       // Zakończono wysyłanie pytań, wyślij kod gry
       else if(prefix == MESSAGE::QUESTION_END) {
            this->sendData(clientFd, std::to_string(clients[clientFd].getGameCode()));
            printf("[INFO] Game code (%d) sent to (%d)\n", clients[clientFd].getGameCode(), clientFd);
        }

       // Wysłano część jednego z pytań
       else {
            const int gameCode = clients[clientFd].getGameCode();

            // Otrzymano pytanie pytania
            if(prefix == MESSAGE::QUESTION_QUESTION) {
                this->games[gameCode].getQuestions().emplace_back();
                this->games[gameCode].getQuestions().back().setQuestion(message);
                this->games[gameCode].getQuestions().back().setNumber((int) games[gameCode].getQuestions().size());
            }

            else if(prefix == MESSAGE::QUESTION_ANSWER_A) {
                this->games[gameCode].getQuestions().back().setAnswerA(message);
            }

            else if(prefix == MESSAGE::QUESTION_ANSWER_B) {
                this->games[gameCode].getQuestions().back().setAnswerB(message);
            }

            else if(prefix == MESSAGE::QUESTION_ANSWER_C) {
                this->games[gameCode].getQuestions().back().setAnswerC(message);
            }

            else if(prefix  == MESSAGE::QUESTION_ANSWER_D) {
                this->games[gameCode].getQuestions().back().setAnswerD(message);
            }

            else if(prefix  == MESSAGE::QUESTION_CORRECT) {
                this->games[gameCode].getQuestions().back().setCorrect(message);
            }

            else {
                printf("[ERROR] Unknown question part type!\n");
                return;
            }
        }
    }
    // Ktoś próbuje dołączyć do gry (wpisał kod)
    else if(action == MESSAGE::JOINING) {
        const int submittedCode = this->stringToInt(message);

        for(auto& game : this->games) {

            const int correctCode = game.first;

            // Znaleziono grę o podanym kodzie
            if(submittedCode == correctCode) {

                // Gra już trwa
                if(game.second.isStarted()) {
                    this->sendData(clientFd, msgToStr(MESSAGE::GAME_ALREADY_STARTED));
                    printf("[INFO] Game is already running (gameCodeView)\n");
                    return;
                }

                // Gracz dołącza do gry
                this->sendData(clientFd, msgToStr(MESSAGE::JOINING_OK));
                this->clients[clientFd].setGameCode(correctCode);
                printf("[INFO] Player (%d) typed correct code (%d)\n", clientFd, correctCode);
                return;
            }
        }

        this->sendData(clientFd, msgToStr(MESSAGE::GAME_NOT_EXISTS));
        printf("[INFO] Game with code (%d) not exists\n", submittedCode);
    }

    // Gracz wysłał pseudonim
    else if(action == MESSAGE::NICK_CHOOSING) {
        const std::string& nick = message;
        const int gameCode = clients[clientFd].getGameCode();

        if(games.find(gameCode) == games.end()) {
            printf("[INFO] Game with code (%d) not exists\n", gameCode);
            this->sendData(clientFd, msgToStr(MESSAGE::GAME_NOT_EXISTS));
        }

        // Sprawdź, czy gra już nie trwa!
        if(this->games[gameCode].isStarted()) {
            // Usuwamy gracza z gry
            this->clients[clientFd].setGameCode(0);
            this->sendData(clientFd, msgToStr(MESSAGE::GAME_ALREADY_STARTED));
            printf("[INFO] Game with code (%d) is already running (gameNickView)\n", gameCode);
            return;
        }

        // Sprawdź unikalność pseudonimu w obrębie gry
        for(auto& anotherClient : this->clients) {
            const int anotherClientFd = anotherClient.first;

            // Nie porównuj się ze sobą
            if(clientFd == anotherClientFd) continue;

            if((nick == anotherClient.second.getNick()) && (gameCode == anotherClient.second.getGameCode())) {
                this->sendData(clientFd, msgToStr(MESSAGE::NICK_USED));
                printf("[INFO] Nick (%s) already exists for game with code (%d)\n", nick.c_str(), gameCode);
                return;
            }
        }

        this->clients[clientFd].setNick(nick);

        // Wyślij wiadomość do twórcy gry o nowym graczu
        const int ownerSocket = games[gameCode].getOwnerSocket();

        if(clients.find(ownerSocket) != clients.end()) {
            this->sendData(ownerSocket, msgToStr(MESSAGE::NEW_PLAYER) + nick);
        } else {
            printf("[INFO] Game owner is not present\n");
        }

        this->sendData(clientFd, msgToStr(MESSAGE::NICK_OK));
        printf("[INFO] Nick (%s) for player (%d) set\n", nick.c_str(), clientFd);
    }

    // Właściciel gry wysłał sygnał do rozpoczęcia gry
    else if(action == MESSAGE::GAME_START) {
        const int gameCode = clients[clientFd].getGameCode();
        // Ktoś nie jest właścicielem, a próbuje rozpocząć grę!
        if(games[gameCode].getOwnerSocket() != clientFd) {
            printf("[INFO] Permission denied!\n");
            return;
        }

        this->games[gameCode].setStarted(true);

        // Wyślij sygnał start
        for(auto& client : clients) {
            if(client.first == clientFd) continue;

            if(client.second.getGameCode() == gameCode) {
                this->sendData(client.first, msgToStr(MESSAGE::GAME_START));
            }
        }

        printf("[INFO] Game owner started game with code (%d)\n", gameCode);

        for(auto& client : clients) {
            if(client.second.getGameCode() == gameCode) {
                this->sendData(client.first, msgToStr(MESSAGE::QUESTION));
                this->sendData(client.first,  games[gameCode].getQuestions()[0].getQuestion());
                this->sendData(client.first, games[gameCode].getQuestions()[0].getAnswerA());
                this->sendData(client.first,  games[gameCode].getQuestions()[0].getAnswerB());
                this->sendData(client.first,  games[gameCode].getQuestions()[0].getAnswerC());
                this->sendData(client.first, games[gameCode].getQuestions()[0].getAnswerD());
                this->sendData(client.first,  "30");
            }
        }

        this->sendData(clientFd, games[gameCode].getQuestions()[0].getCorrect());

    } else if(action == MESSAGE::ANSWER) {
        const int submittedQuestionNumber = std::stoi(message.substr(0, 3));
        const std::string submittedAnswer = message.substr(3, 1);
        const int gameCode = clients[clientFd].getGameCode();

        // Odpowiedź spóźniona
        if(submittedQuestionNumber != games[gameCode].getCurrentQuestion()) {
            printf("[INFO] Player (%s) sent answer for question (%d) in game (%d) too late\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
        } else if(games[gameCode].getQuestions()[submittedQuestionNumber].getCorrect() == submittedAnswer) {
            clients[clientFd].givePoint();
            printf("[INFO] Player (%s) sent correct answer for question (%d) in game (%d)\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
        } else {
            printf("[INFO] Player (%s) sent incorrect answer for question (%d) in game (%d)\n",
                   clients[clientFd].getNick().c_str(), submittedQuestionNumber, gameCode);
        }
    }

    else {
        printf("[ERROR] Unknown message (%s))\n", message.c_str());
    }

}

/**
 * Uruchamia serwer
 */
void Server::run() {

    this->socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if(this->socketFd == -1) {
        perror("[ERROR] socket()");
        exit(EXIT_FAILURE);
    }

    // Umożliwiaj ponownie bindowanie od razu
    const int opt = 1;
    if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == - 1) {
        this->terminate("setsockopt()");
    }

    this->address.sin_addr.s_addr = htonl(INADDR_ANY);
    this->address.sin_port = htons(SERVER_PORT);
    this->address.sin_family = AF_INET;

    if(bind(this->socketFd, (sockaddr*)&this->address, sizeof(sockaddr_in)) == -1) {
        this->terminate("bind()");
    }

    if(listen(this->socketFd, 32) == -1) {
        this->terminate("listen()");
    }

    printf("[INFO] Server is listening on ADDRESS: %s | PORT: %d\n",
           inet_ntoa(this->address.sin_addr),
           htons(this->address.sin_port));

    pollfds.push_back({this->socketFd, POLLIN, 0});

    // Główna pętla programu
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(true) {
        int error_flag = poll(pollfds.data(), pollfds.size(), -1);

        // Przekroczono czas oczekiwania na zdarzenie (trzeci argument poll)
        if(error_flag == 0) {
            this->terminate("poll() timeout");
        }

        // Błąd poll!
        if(error_flag == -1) {
            this->terminate("poll()");
        }

        // Uwaga na iterowanie z jednoczesną modyfikacją!
        auto pollfds_size = pollfds.size();
        for(std::vector<pollfd>::size_type i = 0; i < pollfds_size; ++i) {

            // Nic wartego uwagi się nie wydarzyło
            if(pollfds[i].revents == 0) continue;

            // Wszystko inne niż POLLIN to błąd!
            if(pollfds[i].revents != POLLIN) {
                printf("[ERROR] revents != POLLIN\n");
            }

            // Zgłoszenie na gnieździe nasłuchującym
            if(pollfds[i].fd == this->socketFd) {
                int clientSocket = this->connectClient();
                if(clientSocket != -1) {
                    this->pollfds.push_back(pollfd{clientSocket, POLLIN, 0});
                    pollfds_size = pollfds.size();
                }
            } else {
                int clientFd = pollfds[i].fd;

                // Odbierz długość wiadomości
                std::string messageLength;
                size_t bytes = this->readData(clientFd, HEADER, messageLength);
                if(bytes == 0) {
                    this->disconnectClient(clientFd);
                    pollfds.erase(pollfds.begin() + (long)i);
                    pollfds_size = pollfds.size();
                    --i;
                }

                if(bytes == 0 || bytes == -1) continue;

                // Konwersja string -> int
                int length = stringToInt(messageLength);
                if(length == -1) continue;

                // Odbierz wiadomość o długości przesłanej w nagłówku
                std::string message;
                bytes = this->readData(clientFd, length, message);
                if(bytes == 0) {
                    this->disconnectClient(clientFd);
                    pollfds.erase(pollfds.begin() + (long)i);
                    pollfds_size = pollfds.size();
                    --i;
                }

                if(bytes == 0 || bytes == -1) continue;

                // Podejmij akcję w zależności od wiadomości
                this->makeAction(message, clientFd);
            }


        }
    }
    #pragma clang diagnostic pop
}