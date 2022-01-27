package com.company;

import javax.swing.*;
import java.util.LinkedHashMap;
import java.util.Objects;

public class OwnerView {
    private JLabel codeLabel;
    private JLabel playerNumberLabel;
    protected JPanel panel;
    private JButton playButton;
    private JLabel questionLabel;
    private JLabel answerALabel;
    private JLabel answerBLabel;
    private JLabel answerCLabel;
    private JLabel answerDLabel;
    private JLabel answerCorrectLabel;
    private JLabel timeLabel;
    private JPanel rankPanel;
    private JPanel ownerRankPanel;

    private final Thread thread;
    private boolean started = false;
    private int time = 0;
    private Timer timer;
    private boolean autoNext = false;

    private LinkedHashMap<String, Player> players = new LinkedHashMap<>();

    public OwnerView(String gameCode) {
        codeLabel.setText(gameCode);
        thread = new Thread(new Updater());
        thread.start();

        playButton.addActionListener(actionEvent -> {
            if(!started) {
                if(players.size() > 0) {
                    Application.connection.send(Message.GAME_START.toString());
                    started = true;
                    playButton.setText("Następne pytanie");
                } else {
                    JOptionPane.showMessageDialog(
                            null,
                            "Musi być co najmniej jeden gracz!",
                            "Błąd",
                            JOptionPane.INFORMATION_MESSAGE);
                }
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

                if(Objects.equals(type, Message.NEW_PLAYER.toString())) {
                    String nick = message.substring(2);

                    players.put(nick, new Player(nick, 0));

                    SwingUtilities.invokeLater(() -> {
                        rankPanel.setLayout(new BoxLayout(rankPanel, BoxLayout.Y_AXIS));
                        rankPanel.add(players.get(nick).getLabel());
                        playerNumberLabel.setText(String.valueOf(players.size()));
                        ownerRankPanel.setLayout(new BoxLayout(ownerRankPanel, BoxLayout.Y_AXIS));
                        players.get(nick).getAnswersLabel().setText("<html>" + players.get(nick).getAnswers() + "</html>");
                        ownerRankPanel.add(players.get(nick).getAnswersLabel());
                    });

                    System.out.printf("[INFO] New player (%s)\n", nick);
                } else if(Objects.equals(type, Message.QUESTION.toString())) {
                    String question = Application.connection.receive();
                    String answerA = Application.connection.receive();
                    String answerB = Application.connection.receive();
                    String answerC = Application.connection.receive();
                    String answerD = Application.connection.receive();
                    int newTime = Integer.parseInt(Application.connection.receive());
                    String correct = Application.connection.receive();
                    time = newTime;

                    SwingUtilities.invokeLater(() -> {
                        questionLabel.setText(question);
                        answerALabel.setText(answerA);
                        answerBLabel.setText(answerB);
                        answerCLabel.setText(answerC);
                        answerDLabel.setText(answerD);
                        answerCorrectLabel.setText(correct.toUpperCase());
                        timeLabel.setText(String.valueOf(time));
                        playButton.setEnabled(false);
                    });

                    if(timer != null) {
                        timer.restart();
                    } else {
                        timer = new Timer(1000, actionEvent -> {
                            if(time > 0) timeLabel.setText(String.valueOf(--time));
                            else {
                                if(!autoNext) playButton.setEnabled(true);
                                else Application.connection.send(Message.NEXT_QUESTION.toString());
                            }
                        });

                        timer.start();
                    }


                } else if(Objects.equals(type, Message.QUESTION_END.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Gra została zakończona",
                            "Koniec",
                            JOptionPane.INFORMATION_MESSAGE);
                    return;
                } else if(Objects.equals(type, Message.PLAYERS_RANK.toString())) {
                    String content = message.substring(2);
                    int point = Integer.parseInt(content.substring(0, 1));
                    String nick = content.substring(1);

                    players.get(nick).setPoints(players.get(nick).getPoints() + point);
                    SwingUtilities.invokeLater(() -> players.get(nick).updateLabel());
                } else if(Objects.equals(type, Message.OWNER_RANK.toString())) {
                    String content = message.substring(2);
                    String number = String.valueOf(Integer.parseInt(content.substring(0, 3))+1);
                    String answer = content.substring(3, 4).toUpperCase();
                    String nick = content.substring(4);

                    SwingUtilities.invokeLater(() -> {
                        if(Objects.equals(answerCorrectLabel.getText(), answer))
                            players.get(nick).addAnswer("<font color=green>" + number + answer + "</font>");
                        else if(answer.equals("X"))
                            players.get(nick).addAnswer(number + answer);
                        else
                            players.get(nick).addAnswer("<font color=red>" + number + answer + "</font>");
                        players.get(nick).getAnswersLabel().setText("<html>" + players.get(nick).getAnswers() + "</html>");
                    });
                } else if(Objects.equals(type, Message.AUTO_NEXT.toString())) {
                    String content = message.substring(2, 3);
                    autoNext = (content.equals("y"));
                    if(autoNext) playButton.setEnabled(false);
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
