cmake_minimum_required(VERSION 3.0)
project(sieci_projekt)

set(CMAKE_CXX_STANDARD 17)

add_executable(sieci_projekt main.cpp Server.cpp Server.h Client.cpp Client.h Game.cpp Game.h Question.cpp Question.h Message.h helpers.h)
