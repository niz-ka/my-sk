package com.company;

import javax.swing.*;
import java.util.Objects;

public class Welcome {
    private JTextArea codeTextArea;
    private JButton joinButton;
    private JButton createButton;
    protected JPanel panel;

    public Welcome() {

        createButton.addActionListener(actionEvent -> {
            if (Application.connection != null)
                Application.frame.setApplicationPanel(new GameCreation().panel);
        });

        joinButton.addActionListener(actionEvent -> {
            if (Application.connection != null) {

                String gameCode = codeTextArea.getText();

                if (gameCode.length() != 4 || !gameCode.matches("\\d+")) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Kod gry musi składać się z czterech cyfr",
                            "Błąd",
                            JOptionPane.INFORMATION_MESSAGE);
                    return;
                }

                Application.connection.send(Message.JOINING + gameCode);
                String status = Application.connection.receive();

                if (Objects.equals(status, Message.GAME_NOT_EXISTS.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Błędny kod gry",
                            "Błąd",
                            JOptionPane.INFORMATION_MESSAGE);
                } else if (Objects.equals(status, Message.JOINING_OK.toString())) {
                    System.out.println("[INFO] Joining");
                    Application.frame.setApplicationPanel(new NickView().panel);
                } else if (Objects.equals(status, Message.GAME_ALREADY_STARTED.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Gra już się toczy!",
                            "Błąd",
                            JOptionPane.INFORMATION_MESSAGE);
                } else if (Objects.equals(status, Message.CONNECTION_LOST.toString())) {
                    JOptionPane.showMessageDialog(
                            null,
                            "Utracono połączenie!",
                            "Błąd",
                            JOptionPane.ERROR_MESSAGE);
                } else {
                    System.out.printf("[ERROR] Undefined message (%s)\n", status);
                }
            }
        });
    }
}
