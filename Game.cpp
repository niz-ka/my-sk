//
// Created by kamil on 23.01.2022.
//

#include "Game.h"

Game::Game() : ownerSocket(0), questions({}) {}
Game::Game(int socket) : ownerSocket(socket) {}

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