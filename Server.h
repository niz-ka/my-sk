//
// Created by kamil on 08.01.2022.
//

#ifndef SIECI_PROJEKT_SERVER_H
#define SIECI_PROJEKT_SERVER_H


#include <netinet/in.h>
#include <unordered_map>
#include <string>
#include <sys/poll.h>
#include <vector>
#include "Client.h"
#include "Game.h"

class Server {
    int socketFd;
    sockaddr_in address;
    std::unordered_map<int, Client> clients; // kluczem deskryptor
    std::vector<pollfd> pollfds;
    std::unordered_map<int, Game> games; // kluczem kod gry
public:
    Server();
    void run();

private:
    void terminate(const std::string& description);
    size_t getNumberOfClients() const;
    void disconnectClient(int clientFd);
    int connectClient();
    static size_t readData(int clientFd, int length, std::string& data);
    static int stringToInt(const std::string& number);
    void makeAction(const std::string &message, const int clientFd);
    size_t sendData(int socket, const std::string& data);
};


#endif //SIECI_PROJEKT_SERVER_H
