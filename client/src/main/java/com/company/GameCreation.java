package com.company;

import javax.swing.*;
import java.util.ArrayList;

public class GameCreation {
    protected JPanel panel;
    private JTextField question;
    private JTextField answerA;
    private JTextField answerB;
    private JTextField answerC;
    private JTextField answerD;
    private JRadioButton correctA;
    private JRadioButton correctB;
    private JRadioButton correctC;
    private JRadioButton correctD;
    private JButton newQuestionButton;
    private JButton createButton;
    private JLabel numberLabel;

    private int questionNumber = 1;
    private final ArrayList<Question> questions = new ArrayList<>();
    ButtonGroup correctButtons = new ButtonGroup();


    public GameCreation() {
        correctButtons.add(correctA);
        correctButtons.add(correctB);
        correctButtons.add(correctC);
        correctButtons.add(correctD);

        newQuestionButton.addActionListener(actionEvent -> {
            if(question.getText().isEmpty() ||
                    answerA.getText().isEmpty() ||
                    answerB.getText().isEmpty() ||
                    answerC.getText().isEmpty() ||
                    answerD.getText().isEmpty()
            ) {
                JOptionPane.showMessageDialog(
                        null,
                        "Pytanie oraz odpowiedzi nie mogą być puste.",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
                return;
            }

            questions.add(new Question(
                    questionNumber,
                    question.getText(),
                    answerA.getText(),
                    answerB.getText(),
                    answerC.getText(),
                    answerD.getText(),
                    correctButtons.getSelection().getActionCommand()
            ));
            ++questionNumber;

            question.setText("");
            answerA.setText("");
            answerB.setText("");
            answerC.setText("");
            answerD.setText("");
            correctA.setSelected(true);
            numberLabel.setText("Pytanie " + questionNumber);
        });

        createButton.addActionListener(actionEvent -> {
            if(questions.size() == 0) {
                JOptionPane.showMessageDialog(
                        null,
                        "Proszę dodać co najmniej 1 pytanie.",
                        "Błąd",
                        JOptionPane.INFORMATION_MESSAGE);
                return;
            }

            Application.connection.send(Message.QUESTION.toString() + Message.QUESTION_START);
            for(Question question : questions) {
                String prefix = Message.QUESTION.toString();
                Application.connection.send(prefix + Message.QUESTION_QUESTION + question.getQuestion());
                Application.connection.send(prefix + Message.QUESTION_ANSWER_A + question.getAnswerA());
                Application.connection.send(prefix + Message.QUESTION_ANSWER_B + question.getAnswerB());
                Application.connection.send(prefix + Message.QUESTION_ANSWER_C + question.getAnswerC());
                Application.connection.send(prefix + Message.QUESTION_ANSWER_D + question.getAnswerD());
                Application.connection.send(prefix + Message.QUESTION_CORRECT + question.getCorrect());
            }
            Application.connection.send(Message.QUESTION.toString() + Message.QUESTION_END);

            String gameCode = Application.connection.receive();
            Application.frame.setApplicationPanel(new OwnerView(gameCode).panel);
        });
    }
}
