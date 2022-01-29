#include "Client.h"

Client::Client() : address({}), gameCode(0), answered(false) {}

Client::Client(sockaddr_in address) : address(address), gameCode(0), answered(false) {}

sockaddr_in* Client::getAddressPointer() {
    return &(this->address);
}

const std::string &Client::getNick() const {
    return nick;
}

void Client::setNick(const std::string& _nick) {
    Client::nick = _nick;
}

int Client::getGameCode() const {
    return gameCode;
}

void Client::setGameCode(int _gameCode) {
    Client::gameCode = _gameCode;
}

bool Client::isAnswered() const {
    return answered;
}

void Client::setAnswered(bool _answered) {
    Client::answered = _answered;
}
