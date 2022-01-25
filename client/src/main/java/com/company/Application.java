package com.company;

import javax.swing.*;

public class Application {

    public static WindowFrame frame;
    public static Connection connection;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            frame = new WindowFrame();

            try {
                connection = new Connection("192.168.0.24", 5050);
            } catch(java.io.IOException ex) {
                JOptionPane.showMessageDialog(
                        null,
                        "Błąd połączenia z serwerem. Proszę spróbować później.",
                        "Błąd",
                        JOptionPane.ERROR_MESSAGE);
                System.out.println("[EXCEPTION] " + ex);
            }
        });

    }
}
