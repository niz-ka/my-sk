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
    bool started;
    int currentQuestion;
    bool autoNextQuestion;
public:
    [[nodiscard]] bool isAutoNextQuestion() const;

    void setAutoNextQuestion(bool autoNextQuestion);

public:
    Game();
    explicit Game(int socket);

    [[nodiscard]] int getCurrentQuestion() const;

    int nextQuestion();

    [[nodiscard]] bool isStarted() const;

    void setStarted(bool Started);

    [[nodiscard]] int getOwnerSocket() const;

    std::vector<Question> &getQuestions();
};


#endif //SIECI_PROJEKT_GAME_H
