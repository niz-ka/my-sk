#ifndef SIECI_PROJEKT_CLIENT_H
#define SIECI_PROJEKT_CLIENT_H

#include <netinet/in.h>
#include <sys/poll.h>
#include <iostream>

class Client {
    sockaddr_in address;
    std::string nick;
    int gameCode;
    bool answered;
public:
    [[nodiscard]] int getGameCode() const;
    void setGameCode(int gameCode);

public:
    [[nodiscard]] const std::string &getNick() const;

    [[nodiscard]] bool isAnswered() const;

    void setAnswered(bool answered);

    void setNick(const std::string &nick);

public:
    Client();
    explicit Client(sockaddr_in address);
    sockaddr_in* getAddressPointer();
};


#endif //SIECI_PROJEKT_CLIENT_H
