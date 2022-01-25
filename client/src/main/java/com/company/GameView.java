package com.company;

import javax.swing.*;

public class GameView {
    private JButton answerCbutton;
    private JButton answerDbutton;
    private JButton answerAbutton;
    private JButton answerBbutton;
    protected JPanel panel;
    private JLabel questionLabel;
    private JLabel timeLabel;
    private JLabel pointsLabel;
    private int time;
    private int questionNumber = 0;

    private void sendAnswer(String answer) {
        String message = String.format("%s%03d%s", Message.ANSWER, questionNumber, answer);
        Application.connection.send(message);
        answerAbutton.setEnabled(false);
        answerBbutton.setEnabled(false);
        answerCbutton.setEnabled(false);
        answerDbutton.setEnabled(false);
        questionLabel.setText("Czekaj na innych ...");
    }

    public GameView() {
        Application.connection.receive();
        String question = Application.connection.receive();
        String answerA = Application.connection.receive();
        String answerB = Application.connection.receive();
        String answerC = Application.connection.receive();
        String answerD = Application.connection.receive();
        time = Integer.parseInt(Application.connection.receive());

        questionLabel.setText(question);
        answerAbutton.setText(answerA);
        answerBbutton.setText(answerB);
        answerCbutton.setText(answerC);
        answerDbutton.setText(answerD);
        timeLabel.setText(String.valueOf(time));

        Timer timer = new Timer(1000, actionEvent -> timeLabel.setText(String.valueOf(--time)));
        timer.start();

        answerAbutton.addActionListener(actionEvent -> sendAnswer("a"));
        answerBbutton.addActionListener(actionEvent -> sendAnswer("b"));
        answerCbutton.addActionListener(actionEvent -> sendAnswer("c"));
        answerDbutton.addActionListener(actionEvent -> sendAnswer("d"));
    }
}
