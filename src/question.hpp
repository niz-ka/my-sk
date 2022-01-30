#pragma once

#include <iostream>

class Question
{
    std::string question;
    std::string answerA;
    std::string answerB;
    std::string answerC;
    std::string answerD;
    std::string correct;
    std::string time;

public:
    [[nodiscard]] const std::string &getTime() const;

    void setTime(const std::string &time);

    Question();

    [[nodiscard]] const std::string &getQuestion() const;

    void setQuestion(const std::string &question);

    [[nodiscard]] const std::string &getAnswerA() const;

    void setAnswerA(const std::string &answerA);

    [[nodiscard]] const std::string &getAnswerB() const;

    void setAnswerB(const std::string &answerB);

    [[nodiscard]] const std::string &getAnswerC() const;

    void setAnswerC(const std::string &answerC);

    [[nodiscard]] const std::string &getAnswerD() const;

    void setAnswerD(const std::string &answerD);

    [[nodiscard]] const std::string &getCorrect() const;

    void setCorrect(const std::string &correct);
};