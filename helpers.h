//
// Created by kamil on 28.01.2022.
//

#ifndef SIECI_PROJEKT_HELPERS_H
#define SIECI_PROJEKT_HELPERS_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cstring>
#include <sys/socket.h>

size_t readData(int clientFd, int length, std::string &data) {
    char *message = new char[length];
    size_t bytes;
    size_t bytesRead = 0;

    while (bytesRead < length) {
        bytes = recv(clientFd, message + bytesRead, length - bytesRead, MSG_DONTWAIT);

        if (bytes == -1) {
            delete[] message;
            perror("[ERROR] recv()");
            return -1;
        }

        if (bytes == 0) {
            delete[] message;
            return 0;
        }

        bytesRead += bytes;
    }

    data = std::string(message, length);
    delete[] message;

    return bytesRead;
}

int stringToInt(const std::string &number) {

    int numberInt;

    try {
        numberInt = std::stoi(number);
    } catch (const std::invalid_argument &ia) {
        printf("[ERROR] Conversion impossible (%s)\n", number.c_str());
        return -1;
    } catch (const std::out_of_range &ofr) {
        perror("[ERROR] Conversion out ouf range");
        return -1;
    }

    return numberInt;

}

size_t sendData(int socket, const std::string &data) {
    if (data.length() > 10000) {
        printf("[ERROR] Message too long\n");
        return -1;
    }

    const char *tmp = data.c_str();

    char *buf = new char[strlen(tmp) + 4];
    if (sprintf(buf, "%04zu%s", strlen(tmp), tmp) < 0) {
        perror("[ERROR] sprintf()");
        delete[] buf;
        return -1;
    }

    const size_t length = strlen(buf);

    size_t total = 0;
    size_t bytes;
    size_t bytesLeft = length;

    while (total < length) {
        bytes = send(socket, buf + total, bytesLeft, MSG_DONTWAIT);
        if (bytes == -1) {
            perror("[ERROR] send()");
            delete[] buf;
            return -1;
        }

        total += bytes;
        bytesLeft -= bytes;
    }

    delete[] buf;
    return total;
}

#endif //SIECI_PROJEKT_HELPERS_H
