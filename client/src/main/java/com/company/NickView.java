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
            Application.connection.send(Message.NICK_CHOOSING.asChar() + nick);
            String message = Application.connection.receive();

            if(message.charAt(0) == Message.NICK_USED.asChar()) {
                JOptionPane.showMessageDialog(
                        null,
                        "Pseudonim zajęty! Proszę wybrać inny",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
                return;
            } else if(message.charAt(0) == Message.GAME_ALREADY_STARTED.asChar()) {
                JOptionPane.showMessageDialog(
                        null,
                        "Gra już się toczy!",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
            }

            nickTextField.setVisible(false);
            playButton.setVisible(false);
            infoField.setText("Oczekiwanie na rozpoczęcie gry");
        });
    }
}
