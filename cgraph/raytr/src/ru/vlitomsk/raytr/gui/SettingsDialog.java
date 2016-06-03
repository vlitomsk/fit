package ru.vlitomsk.raytr.gui;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

import javax.swing.*;
import java.awt.event.*;
import java.util.Set;

class JSliderEditor extends JPanel {
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

/**
 * Created by vas on 31.05.2016.
 */
public class SettingsDialog extends JDialog {
    private Color bgcol;
    JTextField gammafld;
    JSliderEditor deptheditor, qualeditor;
    SettingsResult oldSettings;
    SettingsResult result = null;
    public SettingsDialog(Frame owner, SettingsResult oldSettings) {
        super(owner,true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        setTitle("Render Settings");
        this.oldSettings = oldSettings;
        bgcol = oldSettings.bgColor;

        JButton setcolbtn;
        add(setcolbtn = new JButton("Bg color"));
        setcolbtn.addActionListener(e -> {
            Color col = JColorChooser.showDialog(null, "bg color", bgcol);
            if (col != null) {
                bgcol = col;
            }
        });

        add(new JLabel("Gamma:"));
        add(gammafld = new JTextField(String.valueOf(oldSettings.gamma)));
        add(deptheditor = new JSliderEditor("Depth", 1, 3, oldSettings.depth));
        add(qualeditor = new JSliderEditor("Quality", 0, 2, oldSettings.quality));
        JButton okbtn = new JButton("OK"), cancbtn = new JButton("Cancel");
        JPanel okcanc = new JPanel();
        okbtn.addActionListener(e -> {
            try {
                result = new SettingsResult(bgcol, Double.parseDouble(gammafld.getText()), deptheditor.getValue(), qualeditor.getValue());
            } catch (NumberFormatException ex) {
                JOptionPane.showMessageDialog(SettingsDialog.this, "Gamma must be double val",
                        "Error", JOptionPane.ERROR_MESSAGE);
                result = null;
                return;
            }
            dispose();
        });
        cancbtn.addActionListener(e -> {
            result = null;
            dispose();
        });
        okcanc.add(okbtn);
        okcanc.add(cancbtn);
        add(okcanc);
        pack();
        setLocationRelativeTo(owner);
        setVisible(true);
    }

    public SettingsResult getResult() {
        return result;
    }

}
