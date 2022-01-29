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
    int port;
    sockaddr_in address;
    std::unordered_map<int, Client> clients; // kluczem deskryptor
    std::vector<pollfd> pollfds;
    std::unordered_map<int, Game> games; // kluczem kod gry
public:
    explicit Server(int port);
    void run();

private:
    void terminate(const std::string& description);
    size_t getNumberOfClients() const;
    void disconnectClient(int clientFd, unsigned long& i);
    int connectClient();
    void makeAction(std::string &message, int clientFd);
};


#endif //SIECI_PROJEKT_SERVER_H
