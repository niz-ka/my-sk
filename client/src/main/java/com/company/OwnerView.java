package com.company;

import javax.swing.*;
import java.util.Objects;

public class OwnerView {
    private JLabel codeLabel;
    private JLabel playerNumberLabel;
    protected JPanel panel;
    private JTextArea playersTextArea;
    private JButton playButton;

    private int players = 0;
    private final Thread thread;

    public OwnerView(String gameCode) {
        codeLabel.setText(gameCode);
        thread = new Thread(new PlayersUpdater());
        thread.start();

        playButton.addActionListener(actionEvent -> {
            Application.connection.send(Message.GAME_START.toString());

            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });
    }

    class PlayersUpdater implements Runnable {
        @Override
        public void run() {
            while(true) {
                String newPlayer = Application.connection.receive();
                if(Objects.equals(newPlayer, Message.GAME_START.toString())) {
                    break;
                }
                ++players;
                SwingUtilities.invokeLater(() -> {
                    playersTextArea.append("\n" + newPlayer);
                    playerNumberLabel.setText(String.valueOf(players));
                });
            }
        }
    }
}
