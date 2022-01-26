package com.company;

import javax.swing.*;

public class Player {
    private String nick;

    private int points;
    private JLabel label;

    private String answers;
    private JLabel answersLabel;


    public Player(String nick, int points) {
        this.nick = nick;
        this.points = points;
        this.label = new JLabel();
        this.answersLabel = new JLabel();
        this.updateLabel();
    }

    public String getAnswers() {
        return answers;
    }

    public void setAnswers(String answers) {
        this.answers = answers;
    }

    public JLabel getAnswersLabel() {
        return answersLabel;
    }

    public void setAnswersLabel(JLabel answersLabel) {
        this.answersLabel = answersLabel;
    }

    public void updateLabel() {
        label.setText(nick + " " + points);
    }

    public String getNick() {
        return nick;
    }

    public void setNick(String nick) {
        this.nick = nick;
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

    public void setLabel(JLabel label) {
        this.label = label;
    }
}
