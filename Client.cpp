//
// Created by kamil on 08.01.2022.
//

#include "Client.h"

Client::Client() : socketFd(0), address({}), gameOwnerSocket(-10) {}

Client::Client(int socketFd, sockaddr_in address)
: socketFd(socketFd), address(address), gameOwnerSocket(-10) {}

int Client::getSocketFd() const {
    return this->socketFd;
}

sockaddr_in* Client::getAddressPointer() {
    return &(this->address);
}

const std::string &Client::getNick() const {
    return nick;
}

void Client::setNick(const std::string &nick) {
    Client::nick = nick;
}

int Client::getGameOwnerSocket() const {
    return gameOwnerSocket;
}

void Client::setGameOwnerSocket(int gameOwnerSocket) {
    Client::gameOwnerSocket = gameOwnerSocket;
}
