//
// Created by kamil on 23.01.2022.
//

#include "Question.h"

Question::Question() : number(0)
{}

int Question::getNumber() const {
    return number;
}

void Question::setNumber(int number) {
    Question::number = number;
}

const std::string &Question::getQuestion() const {
    return question;
}

void Question::setQuestion(const std::string &question) {
    Question::question = question;
}

const std::string &Question::getAnswerA() const {
    return answerA;
}

void Question::setAnswerA(const std::string &answerA) {
    Question::answerA = answerA;
}

const std::string &Question::getAnswerB() const {
    return answerB;
}

void Question::setAnswerB(const std::string &answerB) {
    Question::answerB = answerB;
}

const std::string &Question::getAnswerC() const {
    return answerC;
}

void Question::setAnswerC(const std::string &answerC) {
    Question::answerC = answerC;
}

const std::string &Question::getAnswerD() const {
    return answerD;
}

void Question::setAnswerD(const std::string &answerD) {
    Question::answerD = answerD;
}

const std::string &Question::getCorrect() const {
    return correct;
}

void Question::setCorrect(const std::string &correct) {
    Question::correct = correct;
}

void Question::print() {

    printf("Number:%d|Question:%s|AnswerA:%s|AnswerB:%s|AnswerC:%s|AnswerD:%s|Correct:%s\n",
          number, question.c_str(), answerA.c_str(), answerB.c_str(), answerC.c_str(), answerD.c_str(), correct.c_str()
          );
}

const std::string &Question::getTime() const {
    return time;
}

void Question::setTime(const std::string &time) {
    Question::time = time;
}
