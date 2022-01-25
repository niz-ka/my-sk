package com.company;

public enum Message {
    QUESTION(10),
        QUESTION_START(11),
        QUESTION_END (12),
        QUESTION_QUESTION (13),
        QUESTION_ANSWER_A (14),
        QUESTION_ANSWER_B (15),
        QUESTION_ANSWER_C (16),
        QUESTION_ANSWER_D (17),
        QUESTION_CORRECT (18),
    JOINING(19),
    JOINING_OK (20),
    GAME_ALREADY_STARTED(21),
    GAME_NOT_EXISTS(22),
    NICK_CHOOSING(23),
    NICK_USED(24),
    NICK_OK(25),
    GAME_START(26),
    ERROR(27),
    ANSWER(28),
    NEXT_QUESTION(29),
    NEW_PLAYER(30),
    QUESTION_TIME(31);

    private final int value;

    @Override
    public String toString() {
        return String.valueOf(this.value);
    }

    Message(int value) {
        this.value = value;
    }
}