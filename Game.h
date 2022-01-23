//
// Created by kamil on 23.01.2022.
//

#ifndef SIECI_PROJEKT_GAME_H
#define SIECI_PROJEKT_GAME_H

#include <vector>
#include <ostream>
#include "Question.h"

class Game {
    int ownerSocket;
    std::vector<Question> questions;

public:
    Game();
    explicit Game(int socket);

    int getOwnerSocket() const;

    void setOwnerSocket(int ownerSocket);

    std::vector<Question> &getQuestions();

    void setQuestions(const std::vector<Question> &questions);
};


#endif //SIECI_PROJEKT_GAME_H
