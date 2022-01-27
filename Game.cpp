//
// Created by kamil on 23.01.2022.
//

#include "Game.h"

Game::Game() : ownerSocket(0), questions({}), code(0), started(false), currentQuestion(0), autoNextQuestion(false) {}
Game::Game(int socket, int gameCode) : ownerSocket(socket), questions({}), code(gameCode), started(false), currentQuestion(0), autoNextQuestion(false) {}

int Game::getOwnerSocket() const {
    return ownerSocket;
}

void Game::setOwnerSocket(int ownerSocket) {
    Game::ownerSocket = ownerSocket;
}

int Game::nextQuestion() {
    return ++currentQuestion;
}

std::vector<Question> &Game::getQuestions() {
    return questions;
}

void Game::setQuestions(const std::vector<Question> &questions) {
    Game::questions = questions;
}

int Game::getCode() const {
    return code;
}

void Game::setCode(int code) {
    Game::code = code;
}

bool Game::isStarted() const {
    return started;
}

void Game::setStarted(bool started) {
    Game::started = started;
}

int Game::getCurrentQuestion() const {
    return currentQuestion;
}

void Game::setCurrentQuestion(int currentQuestion) {
    Game::currentQuestion = currentQuestion;
}

bool Game::isAutoNextQuestion() const {
    return autoNextQuestion;
}

void Game::setAutoNextQuestion(bool autoNextQuestion) {
    Game::autoNextQuestion = autoNextQuestion;
}
