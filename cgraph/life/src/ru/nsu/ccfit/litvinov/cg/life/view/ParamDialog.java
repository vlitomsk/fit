package ru.nsu.ccfit.litvinov.cg.life.view;

import ru.nsu.ccfit.litvinov.cg.life.model.NewDocParams;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * Created by vas on 16.02.16.
 */
public class ParamDialog extends JDialog {
    NewDocParams par = null;

    public ParamDialog(Frame aFrame) {
        super(aFrame, true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        setTitle("Params");

        add(new JLabel("Thickness: "));
        JTextField ttxt = new JTextField("1", 15);
        add(ttxt);

        JSlider hSlider = new JSlider(5, 81, 40);
        JSlider thickSlider = new JSlider(1, 10, 1);
        add(thickSlider);
        thickSlider.addChangeListener((e) -> {
            ttxt.setText(String.valueOf(thickSlider.getValue()));
            if (thickSlider.getValue() * 2 + 1> hSlider.getValue()) {
                hSlider.setValue(thickSlider.getValue() * 2 + 1);
            }
            hSlider.setMinimum(Math.max(5, thickSlider.getValue() * 2 + 1));
        });

        add(new JLabel("Height: "));
        JTextField htxt = new JTextField("40", 15);
        add(htxt);

        add(hSlider);
        hSlider.addChangeListener((e) -> {
            htxt.setText(String.valueOf(hSlider.getValue()));
            if (thickSlider.getValue() * 2 + 1> hSlider.getValue()) {
                thickSlider.setValue((hSlider.getValue() - 1) / 2);
            }
            thickSlider.setMaximum(Math.min((hSlider.getValue() - 1) / 2, 81));
        });

        JButton btn = new JButton("OK");
        add(btn);
        JButton cbtn = new JButton("Cancel");
        ParamDialog pd = this;
        add(cbtn);
        cbtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                SwingUtilities.invokeLater(()->pd.dispose());
            }
        });
        btn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    int t = Integer.parseInt(ttxt.getText());
                    int h = (Integer.parseInt(htxt.getText()) / 2) * 2 + 1;
                    par = new NewDocParams(0, 0, t, h);
                    dispose();
                } catch (NumberFormatException ex) {
                    JOptionPane.showMessageDialog(ParamDialog.this, "Enter integer values!", "Format error",
                            JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        pack();
        setLocationRelativeTo(aFrame);
        setVisible(true);
    }

    public NewDocParams get() {
        return par;
    }
}
