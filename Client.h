//
// Created by kamil on 08.01.2022.
//

#ifndef SIECI_PROJEKT_CLIENT_H
#define SIECI_PROJEKT_CLIENT_H

#include <netinet/in.h>
#include <sys/poll.h>
#include <iostream>

class Client {
    int socketFd;
    sockaddr_in address;
    std::string nick;
    int gameOwnerSocket;
public:
    int getGameOwnerSocket() const;

    void setGameOwnerSocket(int gameOwnerSocket);

public:
    const std::string &getNick() const;

    void setNick(const std::string &nick);

public:
    Client();
    Client(int socketFd, sockaddr_in address);
    int getSocketFd() const;
    sockaddr_in* getAddressPointer();
};


#endif //SIECI_PROJEKT_CLIENT_H
