//
// Created by kamil on 23.01.2022.
//

#include "Game.h"

Game::Game() : ownerSocket(0), questions({}), code(0), started(false) {}
Game::Game(int socket, int gameCode) : ownerSocket(socket), questions({}), code(gameCode), started(false) {}

int Game::getOwnerSocket() const {
    return ownerSocket;
}

void Game::setOwnerSocket(int ownerSocket) {
    Game::ownerSocket = ownerSocket;
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
