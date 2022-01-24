package com.company;

import javax.swing.*;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import static com.company.Application.frame;

public class Welcome {
    private JTextArea codeTextArea;
    private JButton joinButton;
    private JButton createButton;
    protected JPanel panel;

    public Welcome() {
        createButton.addActionListener(actionEvent -> {
            if(Application.connection != null)
                frame.setApplicationPanel(new GameCreation().panel);
        });

        joinButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if(Application.connection != null) {
                    Application.connection.send("j" + codeTextArea.getText());
                    String status = Application.connection.receive();
                    if(status.equals("jr")) {
                        JOptionPane.showMessageDialog(
                                null,
                                "Błędny kod gry",
                                "Błąd",
                                JOptionPane.INFORMATION_MESSAGE);
                    } else if(status.equals("jo")) {
                        System.out.println("Join to game");
                    } else {
                        System.out.println("[ERROR] Undefined join message!");
                    }
                }
            }
        });
    }
}
