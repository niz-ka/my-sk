package com.company;

import javax.swing.*;
import java.io.InputStream;
import java.util.Properties;

public class Application {

    public static WindowFrame frame;
    public static Connection connection;
    public static String userNick;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            InputStream is = Application.class.getClassLoader().getResourceAsStream("app.config");
            Properties properties = new Properties();

            try {
                properties.load(is);
            } catch(java.io.IOException e) {
                JOptionPane.showMessageDialog(
                        null,
                        "Błąd podczas czytania pliku app.config",
                        "Błąd",
                        JOptionPane.ERROR_MESSAGE);
                System.out.println("[EXCEPTION] " + e);
                return;
            }

            String address = properties.getProperty("app.address");
            int  port = Integer.parseInt(properties.getProperty("app.port"));

            try {
                connection = new Connection(address, port);
            } catch (java.io.IOException ex) {
                JOptionPane.showMessageDialog(
                        null,
                        "Błąd połączenia z serwerem. Sprawdź plik app.config.",
                        "Błąd",
                        JOptionPane.ERROR_MESSAGE);
                System.out.println("[EXCEPTION] " + ex);
                return;
            }

            frame = new WindowFrame();
        });

    }
}
