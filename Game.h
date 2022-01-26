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
    int code;
    bool started;
    int currentQuestion;
public:
    Game();
    explicit Game(int socket, int gameCode);

    int getCurrentQuestion() const;

    void setCurrentQuestion(int currentQuestion);
    int nextQuestion();

    bool isStarted() const;

    void setStarted(bool Started);

    int getOwnerSocket() const;

    void setOwnerSocket(int ownerSocket);

    std::vector<Question> &getQuestions();

    void setQuestions(const std::vector<Question> &questions);

    int getCode() const;

    void setCode(int code);
};


#endif //SIECI_PROJEKT_GAME_H
