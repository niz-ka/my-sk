package com.company;

public class Question {
    private final int number;
    private final String question;
    private final String answerA;
    private final String answerB;
    private final String answerC;
    private final String answerD;
    private final String correct;
    private final String time;

    public Question(int number, String question, String answerA, String answerB, String answerC, String answerD, String correct, String time) {
        this.number = number;
        this.question = question;
        this.answerA = answerA;
        this.answerB = answerB;
        this.answerC = answerC;
        this.answerD = answerD;
        this.correct = correct;
        this.time = time;
    }

    public String getTime() {
        return time;
    }

    public String getQuestion() {
        return question;
    }

    public String getAnswerA() {
        return answerA;
    }

    public String getAnswerB() {
        return answerB;
    }

    public String getAnswerC() {
        return answerC;
    }

    public String getAnswerD() {
        return answerD;
    }

    public String getCorrect() {
        return correct;
    }

    @Override
    public String toString() {
        return "Question{" +
                "number=" + number +
                ", question='" + question + '\'' +
                ", answerA='" + answerA + '\'' +
                ", answerB='" + answerB + '\'' +
                ", answerC='" + answerC + '\'' +
                ", answerD='" + answerD + '\'' +
                ", correct='" + correct + '\'' +
                '}';
    }
}
