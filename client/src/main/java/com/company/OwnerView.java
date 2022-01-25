package com.company;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
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

        playButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                Application.connection.send(String.valueOf(Message.GAME_START));
                try {
                    thread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    class PlayersUpdater implements Runnable {
        @Override
        public void run() {
            while(true) {
                String newPlayer = Application.connection.receive();
                if(newPlayer.charAt(0) == Message.GAME_START.asChar()) {
                    break;
                }
                playersTextArea.append("\n" + newPlayer);
                ++players;
                playerNumberLabel.setText(String.valueOf(players));
            }
        }
    }
}
