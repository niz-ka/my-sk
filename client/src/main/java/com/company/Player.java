package com.company;

import javax.swing.*;

public class Player {
    private final String nick;

    private int points;
    private final JLabel label;

    private String answers;
    private final JLabel answersLabel;


    public Player(String nick, int points) {
        this.nick = nick;
        this.points = points;
        this.label = new JLabel();
        this.answers = nick + " ";
        this.answersLabel = new JLabel(answers);
        this.updateLabel();
    }

    public String getAnswers() {
        return answers;
    }

    public JLabel getAnswersLabel() {
        return answersLabel;
    }

    public void updateLabel() {
        label.setText(nick + " " + points);
    }

    public void addAnswer(String answer) {
        answers += answer + " ";
    }

    public int getPoints() {
        return points;
    }

    public void setPoints(int points) {
        this.points = points;
    }

    public JLabel getLabel() {
        return label;
    }
}
