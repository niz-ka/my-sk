package com.company;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedHashMap;
import java.util.Objects;

public class GameView {
    private JButton answerCbutton;
    private JButton answerDbutton;
    private JButton answerAbutton;
    private JButton answerBbutton;
    protected JPanel panel;
    private JLabel questionLabel;
    private JLabel timeLabel;
    private JLabel questionNumberLabel;
    private JLabel pointsLabel;
    private JPanel rankPanel;
    private JLabel nickLabel;

    private int time;
    private int questionNumber = 0;
    private Thread thread;
    private Timer timer;
    private String correct;
    private int points = 0;

    private LinkedHashMap<String, Player> players = new LinkedHashMap<>();

    public GameView() {
        nickLabel.setText(Application.userNick);
        pointsLabel.setText(String.valueOf(points));

        thread = new Thread(new Updater());
        thread.start();

        answerAbutton.addActionListener(actionEvent -> sendAnswer("a", actionEvent));
        answerBbutton.addActionListener(actionEvent -> sendAnswer("b", actionEvent));
        answerCbutton.addActionListener(actionEvent -> sendAnswer("c", actionEvent));
        answerDbutton.addActionListener(actionEvent -> sendAnswer("d", actionEvent));
    }

    private void setButtonsEnabled(boolean enabled) {
        answerAbutton.setEnabled(enabled);
        answerBbutton.setEnabled(enabled);
        answerCbutton.setEnabled(enabled);
        answerDbutton.setEnabled(enabled);
    }

    private void sendAnswer(String answer, ActionEvent actionEvent) {
        String message = String.format("%s%03d%s", Message.ANSWER, questionNumber - 1, answer);
        Application.connection.send(message);
        setButtonsEnabled(false);
        JButton button = (JButton) actionEvent.getSource();
        String text = button.getText();
        answerAbutton.setText("");
        answerBbutton.setText("");
        answerCbutton.setText("");
        answerDbutton.setText("");
        button.setText(text);
    }

    class Updater implements Runnable {

        @Override
        public void run() {
            while(true) {
                String message = Application.connection.receive();
                String type = message.substring(0, 2);

                if(Objects.equals(type, Message.QUESTION.toString())) {
                    String question = Application.connection.receive();
                    String answerA = Application.connection.receive();
                    String answerB = Application.connection.receive();
                    String answerC = Application.connection.receive();
                    String answerD = Application.connection.receive();
                    time = Integer.parseInt(Application.connection.receive());
                    correct = Application.connection.receive();
                    ++questionNumber;

                    SwingUtilities.invokeLater(() -> {
                        questionLabel.setText(question);
                        answerAbutton.setText(answerA);
                        answerBbutton.setText(answerB);
                        answerCbutton.setText(answerC);
                        answerDbutton.setText(answerD);
                        timeLabel.setText(String.valueOf(time));
                        questionNumberLabel.setText(String.valueOf(questionNumber));
                    });

                    setButtonsEnabled(true);


                    if(timer != null)
                        timer.restart();
                    else {
                        timer = new Timer(1000, actionEvent -> {
                            if(time > 0) timeLabel.setText(String.valueOf(--time));
                            else {
                                // Brak odpowiedzi
                                if(answerAbutton.isEnabled()) {
                                    questionLabel.setText("Brak odpowiedzi :/ Czekaj na innych...");
                                    sendAnswer("x", null);
                                }
                            }
                        });

                        timer.start();
                    }

                }

                else if(Objects.equals(type, Message.QUESTION_END.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Gra została zakończona",
                            "Koniec",
                            JOptionPane.INFORMATION_MESSAGE);
                    return;
                }

                else if(Objects.equals(type, Message.PLAYERS_RANK.toString())) {
                    String content = message.substring(2);
                    int point = Integer.parseInt(content.substring(0, 1));
                    String nick = content.substring(1);

                    if(nick.equals(Application.userNick)) {
                        points += point;

                        SwingUtilities.invokeLater(() -> {
                            pointsLabel.setText(String.valueOf(points));
                            if(point == 1) {
                                questionLabel.setText("<html><font color=green>DOBRZE :) Czekaj na innych ...</font></html>");
                            } else {
                                questionLabel.setText("<html><font color=red>ŹLE :( Czekaj na innych ...</font></html>");
                            }
                        });

                    }
                    else {
                        players.get(nick).setPoints(players.get(nick).getPoints() + point);
                        SwingUtilities.invokeLater(() -> players.get(nick).updateLabel());
                    }


                } else if(Objects.equals(type, Message.NEW_PLAYER.toString())) {
                    String nick = message.substring(2);
                    players.put(nick, new Player(nick, 0));

                    SwingUtilities.invokeLater(() -> {
                        rankPanel.setLayout(new BoxLayout(rankPanel, BoxLayout.Y_AXIS));
                        rankPanel.add(players.get(nick).getLabel());
                    });
                }
                else {
                    System.out.printf("[ERROR] Undefined message (%s)\n", type);
                }

                SwingUtilities.invokeLater(() -> {
                    Application.frame.revalidate();
                    Application.frame.repaint();
                    Application.frame.pack();
                });
            }
        }
    }
}
