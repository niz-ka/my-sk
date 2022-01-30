#pragma once

#include <vector>
#include <ostream>
#include "question.hpp"

class Game
{
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