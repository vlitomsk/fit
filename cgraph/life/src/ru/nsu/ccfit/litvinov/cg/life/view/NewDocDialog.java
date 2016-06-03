package ru.nsu.ccfit.litvinov.cg.life.view;

import ru.nsu.ccfit.litvinov.cg.life.model.NewDocParams;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * Created by vas on 16.02.16.
 */
public class NewDocDialog extends JDialog {
    NewDocParams par = null;

    public NewDocDialog(Frame aFrame) {
        super(aFrame, true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        setTitle("New document");
        add(new JLabel("M size: "));
        JTextField mtxt = new JTextField("40", 15);
        add(mtxt);

        add(new JLabel("N size: "));
        JTextField ntxt = new JTextField("40", 15);
        add(ntxt);

        add(new JLabel("Thickness: "));
        JTextField ttxt = new JTextField("1", 15);
        add(ttxt);

        add(new JLabel("Height: "));
        JTextField htxt = new JTextField("41", 15);
        add(htxt);

        JButton btn = new JButton("OK");
        add(btn);
        btn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    int m = Integer.parseInt(mtxt.getText());
                    int n = Integer.parseInt(ntxt.getText());
                    int t = Integer.parseInt(ttxt.getText());
                    int h = Integer.parseInt(htxt.getText());
                    par = new NewDocParams(m, n, t, h);
                    dispose();
                } catch (NumberFormatException ex) {
                    JOptionPane.showMessageDialog(NewDocDialog.this, "Enter integer values!", "Format error",
                            JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        JButton cbtn = new JButton("Cancel");
        NewDocDialog pd = this;
        add(cbtn);
        cbtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                SwingUtilities.invokeLater(()->pd.dispose());
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
