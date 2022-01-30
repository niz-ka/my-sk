#include "question.hpp"

Question::Question() = default;

const std::string &Question::getQuestion() const
{
    return question;
}

void Question::setQuestion(const std::string &_question)
{
    Question::question = _question;
}

const std::string &Question::getAnswerA() const
{
    return answerA;
}

void Question::setAnswerA(const std::string &_answerA)
{
    Question::answerA = _answerA;
}

const std::string &Question::getAnswerB() const
{
    return answerB;
}

void Question::setAnswerB(const std::string &_answerB)
{
    Question::answerB = _answerB;
}

const std::string &Question::getAnswerC() const
{
    return answerC;
}

void Question::setAnswerC(const std::string &_answerC)
{
    Question::answerC = _answerC;
}

const std::string &Question::getAnswerD() const
{
    return answerD;
}

void Question::setAnswerD(const std::string &_answerD)
{
    Question::answerD = _answerD;
}

const std::string &Question::getCorrect() const
{
    return correct;
}

void Question::setCorrect(const std::string &_correct)
{
    Question::correct = _correct;
}

const std::string &Question::getTime() const
{
    return time;
}

void Question::setTime(const std::string &_time)
{
    Question::time = _time;
}
