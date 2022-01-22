package com.company;

import javax.swing.*;

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
    }
}
