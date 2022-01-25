//
// Created by kamil on 08.01.2022.
//

#include "Client.h"

Client::Client() : socketFd(0), address({}), gameCode(0), points(0) {}

Client::Client(int socketFd, sockaddr_in address)
: socketFd(socketFd), address(address), gameCode(0), points(0) {}

int Client::getSocketFd() const {
    return this->socketFd;
}

void Client::givePoint() {
    ++points;
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

int Client::getGameCode() const {
    return gameCode;
}

void Client::setGameCode(int gameCode) {
    Client::gameCode = gameCode;
}

int Client::getPoints() const {
    return points;
}

void Client::setPoints(int points) {
    Client::points = points;
}
