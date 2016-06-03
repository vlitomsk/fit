package ru.nsu.ccfit.litvinov.cg.wireframe.show;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

/**
 * Created by v on 4/19/16.
 */
public class JWireframe3D extends JPanel {
    private Domain dom;
    private Projector proj = new Projector();
    private double[][] mat3d = proj.assocMat();

    private void forceRepaint() {
        SwingUtilities.invokeLater(() -> repaint());
    }

    private Segment[] segments;

    public JWireframe3D(Domain dom, Segment[] segments) {
        super();
        this.dom = dom;
        this.segments = segments;
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

//    @Override
//    public void paint(Graphics g) {
//        super.paint(g);
//        Graphics2D g2 = (Graphics2D) g;
//
//        for (Segment seg : segments) {
//            //double ang = Math.PI / 1.8;
//            //seg.rotX(Math.sin(ang), Math.cos(ang));
//            g2.drawLine(dom.mapx(seg.x1()), dom.mapy(seg.y1()), dom.mapx(seg.x2()), dom.mapy(seg.y2()));
//        }
//    }

    private static int rn(double x) {
        return (int) Math.round(x);
    }

    private Vec[] vectors = new Vec[4], cloned = new Vec[4];

    {
        vectors[0] = new Vec(300, 300, 300);
        vectors[1] = new Vec(500, 500, 500);

        vectors[2] = new Vec(700, 500, 400);
        vectors[3] = new Vec(400, 500, 700);
    }

    private static final double coef = 1;
    @Override
    public void paint(Graphics g) {
        super.paint(g);
        Graphics2D g2 = (Graphics2D) g;


        for (int i = 0; i < 4; ++i) {
            cloned[i] = vectors[i].clone();
            cloned[i].lmmult(mat3d);
        }

        for (int i = 0; i < 2; ++i) {
            int x1 = rn(cloned[i * 2].x() * coef);
            int y1 = rn(cloned[i * 2].y() * coef);
            int x2 = rn(cloned[i * 2 + 1].x() * coef);
            int y2 = rn(cloned[i * 2 + 1].y() * coef);
            g2.drawLine(x1, y1, x2, y2);
        }
    }
}
