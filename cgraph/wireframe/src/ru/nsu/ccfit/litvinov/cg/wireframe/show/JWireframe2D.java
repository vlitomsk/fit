package ru.nsu.ccfit.litvinov.cg.wireframe.show;

import javax.swing.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.*;

/**
 * Created by v on 4/19/16.
 */
public class JWireframe2D extends JPanel {
    Domain dom;

    private void forceRepaint() {
        SwingUtilities.invokeLater(() -> repaint());
    }

    private BSpline spline;

    public JWireframe2D(Domain dom, BSpline spline) {
        super();
        this.dom = dom;
        this.spline = spline;
        setBackground(Color.white);
        dom.sizeChanged(getWidth(), getHeight());
        this.addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                super.componentResized(e);
                dom.sizeChanged(getWidth(), getHeight());
                forceRepaint();
            }
        });
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);
        Graphics2D g2 = (Graphics2D) g;
        Segment[] segments = spline.sample(Integer.MAX_VALUE);
        for (Segment seg : segments) {
            double ang = Math.PI / 1.8;
            seg.rotX(Math.sin(ang), Math.cos(ang));
            g2.drawLine(dom.mapx(seg.x1()), dom.mapy(seg.y1()), dom.mapx(seg.x2()), dom.mapy(seg.y2()));
        }
    }
}
