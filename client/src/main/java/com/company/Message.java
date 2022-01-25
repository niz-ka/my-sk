package com.company;

public enum Message {
    QUESTION('m'),
        QUESTION_START('s'),
        QUESTION_END ('e'),
        QUESTION_QUESTION ('q'),
        QUESTION_ANSWER_A ('a'),
        QUESTION_ANSWER_B ('b'),
        QUESTION_ANSWER_C ('c'),
        QUESTION_ANSWER_D ('d'),
        QUESTION_CORRECT ('p'),
    JOINING('j'),
    JOINING_OK ('o'),
    GAME_ALREADY_STARTED('g'),
    GAME_NOT_EXISTS('x'),
    NICK_CHOOSING('n'),
    NICK_USED('f'),
    NICK_OK('v'),
    GAME_START('s');

    @Override
    public String toString() {
        return String.valueOf(asChar);
    }

    public char asChar() {
        return asChar;
    }

    private final char asChar;

    Message(char asChar) {
        this.asChar = asChar;
    }
}