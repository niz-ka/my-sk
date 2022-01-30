#pragma once

enum class MESSAGE : int
{
    QUESTION = 10,
    QUESTION_START = 11,
    QUESTION_END = 12,
    QUESTION_QUESTION = 13,
    QUESTION_ANSWER_A = 14,
    QUESTION_ANSWER_B = 15,
    QUESTION_ANSWER_C = 16,
    QUESTION_ANSWER_D = 17,
    QUESTION_CORRECT = 18,
    JOINING = 19,
    JOINING_OK = 20,
    GAME_ALREADY_STARTED = 21,
    GAME_NOT_EXISTS = 22,
    NICK_CHOOSING = 23,
    NICK_USED = 24,
    NICK_OK = 25,
    GAME_START = 26,
    ERROR = 27,
    ANSWER = 28,
    NEXT_QUESTION = 29,
    NEW_PLAYER = 30,
    QUESTION_TIME = 31,
    PLAYERS_RANK = 32,
    OWNER_RANK = 33,
    AUTO_NEXT = 34,
    GAME_END = 35,
    CONNECTION_LOST = 36
};

std::string msgToStr(MESSAGE message)
{
    return std::to_string(static_cast<int>(message));
}

MESSAGE strToMsg(const std::string &message)
{
    if (message.length() != 2)
    {
        printf("[ERROR] strToMsg() length");
        return MESSAGE::ERROR;
    }

    return static_cast<MESSAGE>(std::stoi(message));
}

MESSAGE extractMsg(std::string &message)
{
    std::string action = message.substr(0, 2);
    message = message.substr(2);
    return strToMsg(action);
}