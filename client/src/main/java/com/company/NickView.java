package com.company;

import javax.swing.*;
import java.util.Objects;

public class NickView {
    protected JPanel panel;
    private JTextField nickTextField;
    private JButton playButton;
    private JLabel infoField;


    public NickView() {
        playButton.addActionListener(actionEvent -> {
            String nick = nickTextField.getText();

            if(nick.length() < 4) {
                JOptionPane.showMessageDialog(
                        null,
                        "Pseudonim musi składać się z co najmniej 4 liter",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
                return;
            }

            Application.connection.send(Message.NICK_CHOOSING + nick);
            String message = Application.connection.receive();

            if(Objects.equals(message, Message.NICK_OK.toString())) {
                Application.userNick = nick;
                nickTextField.setVisible(false);
                playButton.setVisible(false);
                infoField.setText("Oczekiwanie na rozpoczęcie gry");
                Thread gameStarter = new Thread(new GameStarter());
                gameStarter.start();
            }
            else if(Objects.equals(message, Message.NICK_USED.toString())) {
                JOptionPane.showMessageDialog(
                        null,
                        "Pseudonim zajęty! Proszę wybrać inny",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
            } else if(Objects.equals(message, Message.GAME_ALREADY_STARTED.toString())) {
                JOptionPane.showMessageDialog(
                        null,
                        "Gra już się toczy!",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
            } else if(Objects.equals(message, Message.GAME_NOT_EXISTS.toString())) {
                JOptionPane.showMessageDialog(
                        null,
                        "Ta gra została zakończona",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
            } else if(Objects.equals(message, Message.CONNECTION_LOST.toString())) {
                JOptionPane.showMessageDialog(
                        null,
                        "Utracono połączenie!",
                        "Błąd",
                        JOptionPane.ERROR_MESSAGE);
            }
            else {
                System.out.printf("[ERROR] Undefined message (%s)\n", message);
            }
        });
    }

    static class GameStarter implements Runnable {
        @Override
        public void run() {
            String gameStart = Application.connection.receive();

            SwingUtilities.invokeLater(() -> {
                if(Objects.equals(gameStart, Message.GAME_START.toString())) {
                    Application.frame.setApplicationPanel(new GameView().panel);
                } else if(Objects.equals(gameStart, Message.CONNECTION_LOST.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Utracono połączenie!",
                            "Błąd",
                            JOptionPane.ERROR_MESSAGE);
                }
                else {
                    System.out.printf("[ERROR] Message (%s) unknown\n", gameStart);
                }
            });

        }
    }
}
