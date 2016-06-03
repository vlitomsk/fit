package ru.nsu.ccfit.litvinov.cg.isolines;

/**
 * Created by v on 4/1/16.
 */

import java.awt.*;
import java.util.Optional;
import javax.swing.*;

public class SettingsDialog extends JDialog {
    static final String titleString = "Настройки";
    static final String
            kString = "Шагов сетки по X:",
            mString = "Шагов сетки по Y:",
            aString = "Обл. определения X1:",
            bString = "Обл. определения X2:",
            cString = "Обл. определения Y1:",
            dString = "Обл. определения Y2:";
    private JTextField kFld, mFld, aFld, bFld, cFld, dFld;

    private Optional<ModelSettings> result = Optional.empty();

    public Optional<ModelSettings> getResult() {
        return result;
    }

    public SettingsDialog(Frame aFrame) {
        super(aFrame, true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        add(new JLabel(kString));
        add(kFld = new JTextField("0", 20));
        add(new JLabel(mString));
        add(mFld = new JTextField("0", 20));
        add(new JLabel(aString));
        add(aFld = new JTextField("0.0", 20));
        add(new JLabel(bString));
        add(bFld = new JTextField("0.0", 20));
        add(new JLabel(cString));
        add(cFld = new JTextField("0.0", 20));
        add(new JLabel(dString));
        add(dFld = new JTextField("0.0", 20));
        JButton okBtn = new JButton("Принять");
        JButton cancBtn = new JButton("Отменить");
        add(okBtn);
        add(cancBtn);
        okBtn.addActionListener((e) -> {
            okPressed();
            if (result.isPresent())
                dispose();
        });
        cancBtn.addActionListener((e) -> dispose());
        setTitle(titleString);
        pack();
        setLocationRelativeTo(aFrame);
        setVisible(true);
    }

    private void okPressed() {
        int k, m;
        double a, b, c, d;
        try {
            k = Integer.parseUnsignedInt(kFld.getText());
            m = Integer.parseUnsignedInt(mFld.getText());
            a = Double.parseDouble(aFld.getText());
            b = Double.parseDouble(bFld.getText());
            c = Double.parseDouble(cFld.getText());
            d = Double.parseDouble(dFld.getText());
            result = Optional.of(new ModelSettings(k, m, 0, Math.min(a,b), Math.max(a,b), Math.min(c,d), Math.max(c,d)));
        } catch (NumberFormatException e) {
            JOptionPane.showMessageDialog(SettingsDialog.this,
                    "Число шагов должно быть положительным десятичным целым числом;\n"+
                    "Область определения должна задаваться десятичными рациональным\n"+
                    "  числом с разделителем-точкой.",
                    "Ошибка формата ввода", JOptionPane.ERROR_MESSAGE);
        }
    }
}
