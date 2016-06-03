package ru.nsu.ccfit.litvinov.cg.filter;

import javax.swing.*;
import java.awt.*;

public class IntDialog extends JDialog {
    private int[] result = null;

    public IntDialog(Frame owner, String title, int n, int[] mins, int[] maxs, int[] sets, String[] labels) {
        super(owner, true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        setTitle(title);

        if (mins.length != n || maxs.length != n || sets.length != n || labels.length != n) {
            throw new RuntimeException("mins.length != n || maxs.length != n || sets.length != n || labels.length != n");
        }

        JSliderEditor[] editors = new JSliderEditor[n];
        for (int i = 0; i < n; ++i) {
            add(editors[i] = new JSliderEditor(labels[i], mins[i], maxs[i], sets[i]));
        }

        JPanel okcancelPanel = new JPanel();
        okcancelPanel.setLayout(new BoxLayout(okcancelPanel, BoxLayout.X_AXIS));
        JButton okbtn = new JButton("OK");
        JButton cancelbtn = new JButton("Cancel");
        okcancelPanel.add(okbtn);
        okcancelPanel.add(cancelbtn);

        okbtn.addActionListener((e) -> {
            result = new int[n];
            for (int i = 0; i < n; ++i) {
                result[i] = editors[i].getValue();
            }
            dispose();
        });

        cancelbtn.addActionListener((e) -> {
            result = null;
            dispose();
        });

        add(okcancelPanel);

        pack();
        setLocationRelativeTo(owner);
        setVisible(true);
    }

    public int[] getResult() {
        return result;
    }
}
