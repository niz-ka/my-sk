package com.company;

import javax.swing.*;

public class OwnerView {
    private JLabel codeLabel;
    private JLabel playerNumberLabel;
    protected JPanel panel;
    private JTextArea playersextArea;

    public OwnerView(String gameCode) {
        codeLabel.setText(gameCode);
    }
}
