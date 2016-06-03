package ru.nsu.ccfit.litvinov.cg.life.view;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;

/**
 * Created by vas on 13.02.16.
 */
public class JHexagon extends JComponent {
    private int width, height;
    BufferedImage canvas;
    double val;
    private int[][] innerHex = new int[6][2];

    public JHexagon(int height) {
        setHexagonHeight(height);
        canvas = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        canvasGraph = canvas.createGraphics();
    }

    public void setVal(double v) {
        val = v;
    }

    private Color bgcolor = Color.lightGray, fgcolor = Color.darkGray;

    @Override
    public void setBackground(Color bg) {
        this.bgcolor = bg;
        repaint();
    }

    @Override
    public void setForeground(Color fg) {
        this.fgcolor = fg;
    }

    private boolean manualPaint = false;

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        if (manualPaint) {
            fillCanvas(this.canvas);
            g2d.drawImage(this.canvas, null, null);
        } else {
            //g2d.setStroke(new BasicStroke(4));
            for (int i = 0; i < innerHex.length; ++i) {
                int ii = (i + 1) % innerHex.length;
                g2d.setColor(fgcolor);

                g2d.drawLine(innerHex[i][0], innerHex[i][1], innerHex[ii][0], innerHex[ii][1]);
                g2d.setColor(bgcolor);
                final int r = 17;
                g2d.drawOval(width / 2 - r / 2, height / 2 - r / 2, r, r);

                //g2d.drawString(String.valueOf(val), width  /2, height /2);
            }
        }
    }

    Graphics2D canvasGraph;
    private void fillCanvas(BufferedImage canvas) {
        throw new RuntimeException("Unimplemented");
    }

    public void setHexagonHeight(int h) {
        this.height = h;
        this.width = (int) Math.round(height * Math.sqrt(3) / 2.0);
        Dimension sz = new Dimension(this.width, this.height);
        setSize(sz);

        innerHex[0][0] = 0;
        innerHex[0][1] = height / 4;

        innerHex[1][0] = width / 2;
        innerHex[1][1] = 0;

        innerHex[2][0] = width;
        innerHex[2][1] = height / 4;

        innerHex[3][0] = width;
        innerHex[3][1] = 3 * height / 4;

        innerHex[4][0] = width / 2;
        innerHex[4][1] = height;

        innerHex[5][0] = 0;
        innerHex[5][1] = 3 * height / 4;
    }

    private int zcross(int x1, int y1, int x2, int y2) {
        return x1 * y2 - x2 * y1;
    }

    public boolean hasPointPrecise(int px, int py) {
        for (int i = 0; i < innerHex.length; ++i) {
            int ii = (i + 1) % innerHex.length;
            boolean ptOnLeftSide =
                    0 < zcross(innerHex[ii][0] - innerHex[i][0], innerHex[ii][1] - innerHex[i][1],
                               px - innerHex[i][0], py - innerHex[i][1]);
            if (!ptOnLeftSide)
                return false;
        }
        return true;
    }
}
