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
            Application.connection.send("n" + nick);
            if(Objects.equals(Application.connection.receive(), "ne")) {
                JOptionPane.showMessageDialog(
                        null,
                        "Pseudonim zajęty! Proszę wybrać inny",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
                return;
            }

            nickTextField.setVisible(false);
            playButton.setVisible(false);
            infoField.setText("Oczekiwanie na rozpoczęcie gry");
        });
    }
}
