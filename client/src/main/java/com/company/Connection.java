package com.company;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;

public class Connection {
    private final Socket socket;
    private final DataOutputStream out;
    private final DataInputStream in;

    public Connection(String address, int port) throws java.io.IOException {
        socket = new Socket(address, port);
        out = new DataOutputStream(socket.getOutputStream());
        in = new DataInputStream(socket.getInputStream());
    }

    public void close() {
        try {
            in.close();
            out.close();
            socket.close();
        } catch (java.io.IOException ex) {
            System.out.println("[Exception] close()" + ex);
        }
    }

    public void send(String message) {
        if (message.length() >= 10000) {
            System.out.println("[ERROR] Message too long");
            return;
        }

        message = String.format("%04d", message.getBytes().length) + message;

        try {
            out.write(message.getBytes());
        } catch (java.io.IOException ex) {
            System.out.println("[Exception] write()" + ex);
        }
    }

    public String receive() {
        try {
            int length = Integer.parseInt(new String(in.readNBytes(4)));
            return new String(in.readNBytes(length));
        } catch (java.io.IOException ex) {
            System.out.println("[Exception] read" + ex);
            return null;
        }
    }
}
