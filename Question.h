//
// Created by kamil on 23.01.2022.
//

#ifndef SIECI_PROJEKT_QUESTION_H
#define SIECI_PROJEKT_QUESTION_H

#include <iostream>

class Question {
    int number;
    std::string question;
    std::string answerA;
    std::string answerB;
    std::string answerC;
    std::string answerD;
    std::string correct;
public:
    Question();

    int getNumber() const;

    void setNumber(int number);

    const std::string &getQuestion() const;

    void setQuestion(const std::string &question);

    const std::string &getAnswerA() const;

    void setAnswerA(const std::string &answerA);

    const std::string &getAnswerB() const;

    void setAnswerB(const std::string &answerB);

    const std::string &getAnswerC() const;

    void setAnswerC(const std::string &answerC);

    const std::string &getAnswerD() const;

    void setAnswerD(const std::string &answerD);

    const std::string &getCorrect() const;

    void setCorrect(const std::string &correct);

    void print();
};


#endif //SIECI_PROJEKT_QUESTION_H
