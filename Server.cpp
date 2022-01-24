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

// TODO - coś lepszego niż define
#define SERVER_PORT 5050
#define HEADER 4        // Długość nagłówka, ten z kolei określa długość wiadomości

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
    this->clients.erase(clientFd);
    this->games.erase(clientFd);
}

/**
 * Akceptuje nowego klienta i dodaje do listy klientów
 * @return deskryptor klienta lub -1 w przypadku błędu
 */
int Server::connectClient() {
    printf("[INFO] Listening socket is readable\n");

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

    printf("[READ] %s\n", data.c_str());
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
    size_t bytes = 0;
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

    printf("[SEND] %s\n", data.c_str());
    delete [] buf;
    return total;
}

/**
 * Podejmuje akcję w zależności od otrzymanej wiadomości
 * @param message wiadomość przesłana przez klienta
 */
void Server::makeAction(const std::string& message, const int clientFd) {
    //std::cout << "[MESSAGE] " << message << std::endl;

    std::string action = message.substr(0, 1);

    if(action == "m") {
        std::string prefix = message.substr(1, 1);
        if(prefix == "s") {
            if(games.find(clientFd) != games.end()) {
                printf("[ERROR] Game with socket %d already exists!\n", clientFd);
                return;
            }
            this->games[clientFd] = Game(clientFd);
        }
        else if(prefix == "e") {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist6(1000,9999);
            int code = static_cast<int>(dist6(rng));
            this->games[clientFd].setCode(code);
            this->sendData(clientFd, std::to_string(code));
        }
        else {
            std::string type = message.substr(2, 1);
            if(type == "q") {
                this->games[clientFd].getQuestions().emplace_back();
                int number = stringToInt(prefix);
                this->games[clientFd].getQuestions().back().setQuestion(message.substr(3));
                this->games[clientFd].getQuestions().back().setNumber(number);
            }
            else if(type == "a") {
                this->games[clientFd].getQuestions().back().setAnswerA(message.substr(3));
            }

            else if(type == "b") {
                this->games[clientFd].getQuestions().back().setAnswerB(message.substr(3));
            }

            else if(type == "c") {
                this->games[clientFd].getQuestions().back().setAnswerC(message.substr(3));
            }

            else if(type == "d") {
                this->games[clientFd].getQuestions().back().setAnswerD(message.substr(3));
            }

            else if(type == "p") {
                this->games[clientFd].getQuestions().back().setCorrect(message.substr(3));
            }

            else {
                printf("[ERROR] Unknown question part type!\n");
                return;
            }
        }
    } else if(action == "j") {
        int code = this->stringToInt(message.substr(1));
        for(auto& game : this->games) {

            if(game.second.getCode() == code) {
                if(game.second.isStarted()) {
                    this->sendData(clientFd, "js");
                    return;
                }

                this->sendData(clientFd, "jo");
                this->clients[clientFd].setGameOwnerSocket(game.first);
                return;
            }
        }
        this->sendData(clientFd, "jr");
    } else if(action == "n") {
        std::string nick = message.substr(1);

        if(this->games[this->clients[clientFd].getGameOwnerSocket()].isStarted()) {
            this->clients[clientFd].setGameOwnerSocket(-10);
            this->sendData(clientFd, "js");
            return;
        }

        for(auto& client : this->clients) {

            if((clientFd != client.first) && nick == client.second.getNick()) {
                this->sendData(clientFd, "ne");
                return;
            }
        }
        this->clients[clientFd].setNick(nick);
        this->sendData(clients[clientFd].getGameOwnerSocket(), nick);
        this->sendData(clientFd, "no");

    } else if(action == "s") {
        this->games[clientFd].setStarted(true);
        this->sendData(clientFd, "s");
    }

    else {
        printf("[ERROR] Unknown message action!\n");
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