package ru.nsu.ccfit.litvinov.cg.filter;

import javax.swing.*;
import java.awt.event.*;

public class JSliderEditor extends JPanel {
    private int min, max, set;
    private JSlider slider;
    public JSliderEditor(String text, int min, int max, int set) {
        this.min = min;
        this.max = max;
        this.set = set;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        slider = new JSlider(min, max, set);
        JTextField tf = new JTextField(5);
        tf.setText(String.valueOf(set));

        slider.addChangeListener((e) -> {
            tf.setText(String.valueOf(slider.getValue()));
        });
        tf.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                super.keyPressed(e);
                try {
                    int v = Integer.parseInt(tf.getText());
                    if (v >= min && v <= max)
                        slider.setValue(v);
                } catch (NumberFormatException ex) {}
            }
        });

        add(new JLabel(text));
        add(slider);
        add(tf);
    }

    public int getValue() {
        return slider.getValue();
    }
}
