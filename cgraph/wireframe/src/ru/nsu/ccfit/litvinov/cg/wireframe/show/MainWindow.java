package ru.nsu.ccfit.litvinov.cg.wireframe.show;

import javax.swing.*;
import javax.swing.border.LineBorder;
import java.awt.*;

/**
 * Created by v on 4/19/16.
 */
public class MainWindow extends JFrame {
    public MainWindow() {
        setTitle("Wireframe show");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        JUtils.resize(this, 800, 600);

        BSpline spline = new BSpline(new double[][] {
                {0, 2}, {1, 1}, {2, 0.5}, {3, 0.25}, {6, 0.25}, {8, 1}, {10, 2.3}, {11, 2.5}
        });
        Domain domain = new Domain(0, 0.25, 11, 2.5);
        RotBody rb = new RotBody(spline.sample(Integer.MAX_VALUE));
        JWireframe3D jp = new JWireframe3D(domain, rb.sample(20));
        jp.setBorder(new LineBorder(Color.black));

        add(jp);

        setVisible(true);
    }
}
