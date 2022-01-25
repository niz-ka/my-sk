package com.company;

import javax.swing.*;
import java.util.Objects;

public class OwnerView {
    private JLabel codeLabel;
    private JLabel playerNumberLabel;
    protected JPanel panel;
    private JTextArea playersTextArea;
    private JButton playButton;
    private JLabel questionLabel;
    private JLabel answerALabel;
    private JLabel answerBLabel;
    private JLabel answerCLabel;
    private JLabel answerDLabel;
    private JLabel answerCorrectLabel;
    private JLabel timeLabel;

    private int players = 0;
    private final Thread thread;
    private boolean started = false;
    private int time = 0;
    private Timer timer;

    public OwnerView(String gameCode) {
        codeLabel.setText(gameCode);
        thread = new Thread(new Updater());
        thread.start();

        playButton.addActionListener(actionEvent -> {
            if(!started) {
                Application.connection.send(Message.GAME_START.toString());
                started = true;
                playButton.setText("Następne pytanie");
            } else {
                Application.connection.send(Message.NEXT_QUESTION.toString());
            }

        });
    }

    class Updater implements Runnable {
        @Override
        public void run() {
            while(true) {
                String message = Application.connection.receive();
                String type = message.substring(0, 2);
                String content = message.substring(2);

                if(Objects.equals(type, Message.NEW_PLAYER.toString())) {
                    ++players;
                    SwingUtilities.invokeLater(() -> {
                        playersTextArea.append("\n" + content);
                        playerNumberLabel.setText(String.valueOf(players));
                    });
                    System.out.printf("[INFO] New player (%s)\n", content);
                } else if(Objects.equals(type, Message.QUESTION.toString())) {
                    String question = Application.connection.receive();
                    String answerA = Application.connection.receive();
                    String answerB = Application.connection.receive();
                    String answerC = Application.connection.receive();
                    String answerD = Application.connection.receive();
                    time = Integer.parseInt(Application.connection.receive());
                    String correct = Application.connection.receive();

                    timer = new Timer(1000, actionEvent -> {
                        if(time > 0) timeLabel.setText(String.valueOf(--time));
                    });

                    timer.start();

                    SwingUtilities.invokeLater(() -> {
                        questionLabel.setText(question);
                        answerALabel.setText(answerA);
                        answerBLabel.setText(answerB);
                        answerCLabel.setText(answerC);
                        answerDLabel.setText(answerD);
                        answerCorrectLabel.setText(correct);
                    });
                } else {
                    System.out.printf("[ERROR] Undefined message (%s)\n", message);
                }


            }
        }
    }
}
