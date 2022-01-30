#pragma once

#include <netinet/in.h>
#include <unordered_map>
#include <string>
#include <sys/poll.h>
#include <vector>
#include "client.hpp"
#include "game.hpp"

class Server
{
    int socketFd;
    int port;
    sockaddr_in address;
    std::vector<pollfd> pollfds;
    std::unordered_map<int, Client> clients; // kluczem deskryptor
    std::unordered_map<int, Game> games;     // kluczem kod gry
public:
    explicit Server(int port);
    void run();

private:
    void terminate(const std::string &description);
    size_t getNumberOfClients() const;
    void disconnectClient(int clientFd, std::size_t i);
    int connectClient();
    void makeAction(std::string &message, int clientFd);
};