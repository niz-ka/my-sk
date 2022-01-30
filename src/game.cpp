#include "game.hpp"

Game::Game() : ownerSocket(0), questions({}), started(false), currentQuestion(0), autoNextQuestion(false) {}
Game::Game(int socket) : ownerSocket(socket), questions({}), started(false), currentQuestion(0), autoNextQuestion(false) {}

int Game::getOwnerSocket() const
{
    return ownerSocket;
}

int Game::nextQuestion()
{
    return ++currentQuestion;
}

std::vector<Question> &Game::getQuestions()
{
    return questions;
}

bool Game::isStarted() const
{
    return started;
}

void Game::setStarted(bool _started)
{
    Game::started = _started;
}

int Game::getCurrentQuestion() const
{
    return currentQuestion;
}

bool Game::isAutoNextQuestion() const
{
    return autoNextQuestion;
}

void Game::setAutoNextQuestion(bool _autoNextQuestion)
{
    Game::autoNextQuestion = _autoNextQuestion;
}
