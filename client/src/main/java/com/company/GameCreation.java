package com.company;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

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
    private ArrayList<Question> questions = new ArrayList<>();
    ButtonGroup correctButtons = new ButtonGroup();


    public GameCreation() {
        correctButtons.add(correctA);
        correctButtons.add(correctB);
        correctButtons.add(correctC);
        correctButtons.add(correctD);

        newQuestionButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
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
            }
        });

        createButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                for(Question question : questions) {
                    String prefix = "m" + question.getNumber();
                    Application.connection.send(prefix + "q" + question.getQuestion());
                    Application.connection.send(prefix + "a" + question.getAnswerA());
                    Application.connection.send(prefix + "b" + question.getAnswerB());
                    Application.connection.send(prefix + "c" + question.getAnswerC());
                    Application.connection.send(prefix + "d" + question.getAnswerD());
                    Application.connection.send(prefix + "p" + question.getCorrect());
                }
            }
        });
    }
}
