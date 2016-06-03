package ru.nsu.ccfit.litvinov.cg.isolines;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

/**
 * Created by v on 4/5/16.
 */
public class JNiceView extends JComponent implements Observer {
    private GridFunctionModel gfm;
    private int w, h;
    private BufferedImage bi ;
    private ViewSettings viewSettings;

    public JNiceView(int w, int h, ViewSettings viewSettings) {
        super();
        this.viewSettings = viewSettings;
        changeSize(w, h);
    }

    public void changeSize(int w, int h) {
        this.w = w;
        this.h = h;
        Dimension dim = new Dimension(w, h);
        setMinimumSize(dim);
        setPreferredSize(dim);
        setSize(dim);
        bi = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        forceRepaint();
    }

    public double getRealX(int compX) {
        ModelSettings ms = gfm.getModelSettings();
        return Math.round(((compX * (ms.b - ms.a)) / w + ms.a)*100.0)/100.0;
    }

    public double getRealX1(int compX) {
        ModelSettings ms = gfm.getModelSettings();
        return ((compX * (ms.b - ms.a)) / w + ms.a);
    }

    public double getRealY(int compY) {
        ModelSettings ms = gfm.getModelSettings();
        return Math.round(((compY * (ms.d - ms.c)) / h + ms.c)*100.0)/100.0;
    }

    public double getRealY1(int compY) {
        ModelSettings ms = gfm.getModelSettings();
        return ((compY * (ms.d - ms.c)) / h + ms.c);
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(w, h);
    }

    double [][] sidespt = new double[4][2];
    int [] sidesIsect = new int[4];

    private double linIsect(double q1, double q2, double qlev, double x1, double xstep) {
        //final double q1 = Math.min(q1_, q2_);
        //final double q2 = Math.max(q1_, q2_);
        return (qlev - q1) * xstep / (q2 - q1) + x1;
    }

    int[] xp = new int[6];
    int[] yp = new int[6];
    private void fill3(Graphics2D g2, double[] p0, double[] p1, double[] p2) {
        xp[0] = (int)Math.round(p0[0]);
        xp[1] = (int)Math.round(p1[0]);
        xp[2] = (int)Math.round(p2[0]);
        yp[0] = (int)Math.round(p0[1]);
        yp[1] = (int)Math.round(p1[1]);
        yp[2] = (int)Math.round(p2[1]);
        g2.fillPolygon(xp, yp, 3);
    }

    private void fill5(Graphics2D g2, double[] p0, double[] p1, double[] p2, double[] p3, double[] p4) {
        xp[0] = (int)Math.round(p0[0]);
        xp[1] = (int)Math.round(p1[0]);
        xp[2] = (int)Math.round(p2[0]);
        xp[3] = (int)Math.round(p3[0]);
        xp[4] = (int)Math.round(p4[0]);
        yp[0] = (int)Math.round(p0[1]);
        yp[1] = (int)Math.round(p1[1]);
        yp[2] = (int)Math.round(p2[1]);
        yp[3] = (int)Math.round(p3[1]);
        yp[4] = (int)Math.round(p4[1]);
        g2.fillPolygon(xp, yp, 5);
    }

    private void fill6(Graphics2D g2, double[] p0, double[] p1, double[] p2, double[] p3, double[] p4, double[] p5) {
        xp[0] = (int)Math.round(p0[0]);
        xp[1] = (int)Math.round(p1[0]);
        xp[2] = (int)Math.round(p2[0]);
        xp[3] = (int)Math.round(p3[0]);
        xp[4] = (int)Math.round(p4[0]);
        xp[5] = (int)Math.round(p5[0]);
        yp[0] = (int)Math.round(p0[1]);
        yp[1] = (int)Math.round(p1[1]);
        yp[2] = (int)Math.round(p2[1]);
        yp[3] = (int)Math.round(p3[1]);
        yp[4] = (int)Math.round(p4[1]);
        yp[5] = (int)Math.round(p5[1]);
        g2.fillPolygon(xp, yp, 6);
    }

    private void fill4(Graphics2D g2, double[] p0, double[] p1, double[] p2, double[] p3) {
        xp[0] = (int)Math.round(p0[0]);
        xp[1] = (int)Math.round(p1[0]);
        xp[2] = (int)Math.round(p2[0]);
        xp[3] = (int)Math.round(p3[0]);
        yp[0] = (int)Math.round(p0[1]);
        yp[1] = (int)Math.round(p1[1]);
        yp[2] = (int)Math.round(p2[1]);
        yp[3] = (int)Math.round(p3[1]);
        g2.fillPolygon(xp, yp, 4);
    }

    private void fillRec(Graphics2D g2, double[] p0, double[] p1) {
        xp[0] = (int)Math.round(p0[0]);
        xp[1] = (int)Math.round(p0[0]);
        xp[2] = (int)Math.round(p1[0]);
        xp[3] = (int)Math.round(p1[0]);
        yp[0] = (int)Math.round(p0[1]);
        yp[1] = (int)Math.round(p1[1]);
        yp[2] = (int)Math.round(p1[1]);
        yp[3] = (int)Math.round(p0[1]);
        g2.fillPolygon(xp,yp,4);
    }

    private int isoarrLen = 0;
    private int[][] isoarr;

    private void drlin(Graphics2D g2, double[] p0, double[] p1) {
        isoarr[isoarrLen][0] = (int)Math.round(p0[0]);
        isoarr[isoarrLen][1] = (int)Math.round(p0[1]);
        isoarr[isoarrLen][2] = (int)Math.round(p1[0]);
        isoarr[isoarrLen][3] = (int)Math.round(p1[1]);
        ++isoarrLen;
    }

    private boolean eq(double x, double y) {
        return Math.abs(x - y) < 1e-7;
    }

    private static final Color gridColor = new Color(120, 120, 120, 100);
    private static final Color labelColor = new Color(10, 255, 10, 100);

    boolean drawColors = true;
    boolean drawInterpol = false;
    boolean drawIsolines = true;
    boolean drawXGrid = true, drawYGrid = true;
    boolean drawXLabels = true, drawYLabels = true;
    private int tickMod = 2;

    public void setTickMod(int tickMod) {
        this.tickMod = tickMod;
        forceRepaint();
    }

    public void setDrawColors(boolean drawColors) {
        this.drawColors = drawColors;
        forceRepaint();
    }

    public void setDrawXGrid(boolean drawGrid) {
        this.drawXGrid = drawGrid;
        forceRepaint();
    }

    public void setDrawYGrid(boolean drawYGrid) {
        this.drawYGrid = drawYGrid;
        forceRepaint();
    }

    public void setDrawGrid(boolean d) {
        drawXGrid = drawYGrid = d;
        forceRepaint();
    }

    public void setDrawIsolines(boolean drawIsolines) {
        this.drawIsolines = drawIsolines;
        forceRepaint();
    }

    public void setDrawXLabels(boolean drawLabels) {
        this.drawXLabels = drawLabels;
        forceRepaint();
    }

    public void setDrawYLabels(boolean drawYLabels) {
        this.drawYLabels = drawYLabels;
        forceRepaint();
    }

    public void setDrawInterpol(boolean drawInterpol) {
        this.drawInterpol = drawInterpol;
        forceRepaint();
    }

    private void forceRepaint() {
        if (gfm == null)
            return;

        Color[] zcolors = viewSettings.legendColors;

        double max = gfm.getMax();
        double min = gfm.getMin();
        ModelSettings ms = gfm.getModelSettings();
        final double xstep = gfm.getXstep();
        final double ystep = gfm.getYstep();
        double domx = ms.b - ms.a;
        double domy = ms.d - ms.c;
        Graphics2D g2 = (Graphics2D) bi.getGraphics();
        g2.clearRect(0, 0, w, h);

        final double img_xstep = (xstep * w) / domx;
        final double img_ystep = (ystep * h) / domy;

        boolean zwas = false;

        isoarrLen = 0;
        ArrayList<Double> zs = userIsoline ? gfm.getZs(userIsoRx, userIsoRy) : gfm.getZs();
        for (int i = 0; i < ms.k  ; ++i) {
            for (int j = 0; j < ms.m; ++j) {
                int k = 0;
                boolean anyIntersections = false;
                int isectCount = 0;
                //double zplane = 0;
                double q11 = 0, q21=0, q12=0, q22=0;
                boolean prevMore = false;
                zwas = false;
                //for (k = 0; k < ms.n; ++k) {
                for (k = 0; k < zs.size() ; ++k) {
                    double zplane = zs.get(k);
                    boolean good = !(userIsoline && zplane == gfm.getUserz());
                    if (!good) {
                        zwas = true;
                    }
                    if (j == 3) {
                    }
//                    if (i ==  && j ==  || i==  && j==) {
//                        System.out.print("");
//                    }
                    isectCount = 0;
                    sidesIsect[0] = sidesIsect[1] = sidesIsect[2] = sidesIsect[3] = 0;
                    //zplane = (k+1) * (max - min) / ms.n + min;
                    q11 = gfm.get(i, j);
                    q21 = gfm.get(i+1,j);
                    q12 = gfm.get(i,j+1);
                    q22 = gfm.get(i+1,j+1);
                    if (q11 >= zplane && q21 >= zplane && q12 >= zplane && q22 >= zplane) {
                        if (drawColors) {
                            g2.setPaint(zcolors[k+(zwas?0:1)]);
                            double[] p1 = {i * img_xstep, j * img_ystep};
                            double[] p2 = {(i + 1) * img_xstep, (j + 1) * img_ystep};
                            fillRec(g2, p1, p2);
                        }
                        prevMore = true;
                        continue;
                    } else if (prevMore) {

                        prevMore = false;
                    }

                    if (q11 <= zplane && q21 <= zplane && q12 <= zplane && q22 <= zplane) {
                        break;
                    }

                    if (q11 <= zplane && q21 >= zplane || q11 >= zplane && q21 <= zplane) { // xstep
                        sidesIsect[0] = ++isectCount;
                        sidespt[0][0] = linIsect(q11, q21, zplane, ms.a + i * xstep, xstep);
                        sidespt[0][1] = j * ystep + ms.c;
                        anyIntersections = true;
                    }
                    if (q21 <= zplane && q22 >= zplane || q21 >= zplane && q22 <= zplane) { // ystep
                        sidesIsect[1] = ++isectCount;
                        sidespt[1][0] = (i + 1) * xstep + ms.a;
                        sidespt[1][1] = linIsect(q21, q22, zplane, ms.c + j * ystep, ystep);
                        anyIntersections = true;
                    }
                    if (q12 <= zplane && q22 >= zplane || q12 >= zplane && q22 <= zplane) { // xstep
                        sidesIsect[2] = ++isectCount;
                        sidespt[2][0] = linIsect(q12, q22, zplane, ms.a + i * xstep, xstep);
                        sidespt[2][1] = (j + 1) * ystep + ms.c;
                        anyIntersections = true;
                    }
                    if (q11 <= zplane && q12 >= zplane || q11 >= zplane && q12 <= zplane) { // ystep
                        sidesIsect[3] = ++isectCount;
                        sidespt[3][0] = i * xstep + ms.a;
                        sidespt[3][1] = linIsect(q11, q12, zplane, ms.a + j * ystep, ystep);
                        anyIntersections = true;
                    }

                    if (anyIntersections) {
                        for (int r = 0; r < 4; ++r) {
                            sidespt[r][0] = (sidespt[r][0] - ms.a) * (double)w / domx;
                            sidespt[r][1] = (sidespt[r][1] - ms.c) * (double)h / domy;
                        }
                        double[] sd0 = sidespt[0];
                        double[] sd1 = sidespt[1];
                        double[] sd2 = sidespt[2];
                        double[] sd3 = sidespt[3];
                        double[] Pt11 = {i * img_xstep, j * img_ystep};
                        double[] Pt21 = {(i+1) * img_xstep, j * img_ystep};
                        double[] Pt22 = {(i+1) * img_xstep, (j+1) * img_ystep};
                        double[] Pt12 = {i * img_xstep, (j + 1) * img_ystep};
                        if (good)
                            g2.setPaint(zcolors[k+1+(zwas?-1:0)]);
                        if (isectCount == 2) {
                           // System.out.println("2");
                            if (sidesIsect[0] > 0 && sidesIsect[3] > 0) {
                                if (good && drawColors) {
                                    if (q11 > zplane)
                                        fill3(g2, Pt11, sd0, sd3);
                                    else
                                        fill5(g2, sd0, Pt21, Pt22, Pt12, sd3);
                                }
                                drlin(g2, sd0, sd3);
                            } else if (sidesIsect[0] > 0 && sidesIsect[1] > 0) {
                                if (good && drawColors) {
                                    if (q21 > zplane)
                                        fill3(g2, Pt21, sd0, sd1);
                                    else
                                        fill5(g2, sd0, sd1, Pt22, Pt12, Pt11);
                                }
                                drlin(g2, sd0, sd1);
                            } else if (sidesIsect[1] > 0 && sidesIsect[2] > 0) {
                                if (good && drawColors) {
                                    if (q22 > zplane)
                                        fill3(g2, Pt22, sd1, sd2);
                                    else
                                        fill5(g2, sd1, sd2, Pt12, Pt11, Pt21);
                                }
                                drlin(g2, sd1, sd2);
                            } else if (sidesIsect[2] > 0 && sidesIsect[3] > 0) {
                                if (good && drawColors) {
                                    if (q12 > zplane)
                                        fill3(g2, Pt12, sd2, sd3);
                                    else
                                        fill5(g2, sd2, sd3, Pt11, Pt21, Pt22);
                                }
                                drlin(g2, sd2, sd3);
                            } else if (sidesIsect[3] > 0 && sidesIsect[1] > 0) {
                                if (good && drawColors) {
                                    if (q11 > zplane)
                                        fill4(g2, Pt11, Pt21, sd1, sd3);
                                    else
                                        fill4(g2, sd3, sd1, Pt22, Pt12);
                                }
                                drlin(g2, sd3, sd1);
                            } else if (sidesIsect[0] > 0 && sidesIsect[2] > 0) {
                                if (good && drawColors) {
                                    if (q11 > zplane)
                                        fill4(g2, sd0, sd2, Pt12, Pt11);
                                    else
                                        fill4(g2, sd2, sd0, Pt21, Pt22);
                                }
                                drlin(g2, sd0, sd2);
                            }
                        } else if (isectCount == 3) {
                            //System.out.println("3");
                            if (eq(sidespt[0][0], sidespt[1][0]) && eq(sidespt[0][1], sidespt[1][1]) || eq(sidespt[3][0], sidespt[2][0]) && eq(sidespt[3][1], sidespt[2][1])) {
                                if (good && drawColors) {
                                    if (q11 > zplane)
                                        fill3(g2, Pt11, Pt21, Pt12);
                                    else
                                        fill3(g2, Pt21, Pt22, Pt12);
                                }
                                drlin(g2, Pt21, Pt12);
                            } else if (eq(sidespt[0][0], sidespt[3][0]) && eq(sidespt[0][1], sidespt[3][1]) || eq(sidespt[2][0], sidespt[1][0]) && eq(sidespt[2][1], sidespt[1][1])) {
                                if (good && drawColors) {
                                    if (q21 > zplane)
                                        fill3(g2, Pt11, Pt21, Pt22);
                                    else
                                        fill3(g2, Pt11, Pt22, Pt12);
                                }
                                drlin(g2, Pt11, Pt22);
                            } else {
                                System.out.println("3!!!");
                            }
                        } else if (isectCount == 4) {
                            //System.out.println("4");
                            if (eq(sidespt[0][0], sidespt[1][0]) && eq(sidespt[0][1], sidespt[1][1]) && eq(sidespt[3][0], sidespt[2][0]) && eq(sidespt[3][1], sidespt[2][1])) {
                                if (good && drawColors) {
                                    if (q11 > zplane)
                                        fill3(g2, Pt11, Pt21, Pt12);
                                    else
                                        fill3(g2, Pt21, Pt22, Pt12);
                                }
                                drlin(g2, Pt21, Pt12);
                            } else if (eq(sidespt[0][0], sidespt[3][0]) && eq(sidespt[0][1], sidespt[3][1]) || eq(sidespt[2][0], sidespt[1][0]) && eq(sidespt[2][1], sidespt[1][1])) {
                                if (good && drawColors) {
                                    if (q21 > zplane)
                                        fill3(g2, Pt11, Pt21, Pt22);
                                    else
                                        fill3(g2, Pt11, Pt22, Pt12);
                                }
                                drlin(g2, Pt11, Pt22);
                            } else if (q11 > zplane && q22 > zplane) {
                                double midz = gfm.getMid(i, j);
                                if (midz < zplane) {
                                    if (good&&drawColors) {
                                        fill3(g2, Pt11, sd0, sd3);
                                        fill3(g2, Pt22, sd1, sd2);
                                    }
                                    drlin(g2, sd0, sd3);
                                    drlin(g2, sd1, sd2);
                                } else {
                                    if (good && drawColors) {
                                        fill6(g2, Pt11, sd0, sd1, Pt22, sd2, sd3);
                                    }
                                    drlin(g2, sd0, sd1);
                                    drlin(g2, sd2, sd3);
                                }
                            } else if (q21 > zplane && q12 > zplane) {
                                double midz = gfm.getMid(i, j);
                                if (midz > zplane) {
                                    if (good && drawColors) {
                                        fill6(g2, sd3, sd0, Pt21, sd1, sd2, Pt12);
                                    }
                                    drlin(g2, sd3, sd0);
                                    drlin(g2, sd2, sd1);
                                } else {
                                    if (good && drawColors) {
                                        fill3(g2, sd0, Pt21, sd1);
                                        fill3(g2, sd2, sd3, Pt12);
                                    }
                                    drlin(g2, sd0, sd1);
                                    drlin(g2, sd2, sd3);
                                }
                            }

                            else{
                                //System.out.println("4!!! " + i + " " + j + " " + zplane);
                            }
                        }
                    }
                }

            }
        }

        //bi.setRGB(100, 100, 100);
        if (drawInterpol) {
            for (int imx = 0; imx < w; ++imx) {
                for (int imy = 0; imy < h; ++imy) {
                    min = Math.min(min, gfm.calcFn(getRealX1(imx), getRealY1(imy)));
                    max = Math.max(max, gfm.calcFn(getRealX1(imx), getRealY1(imy)));
                }
            }
            for (int imx = 0; imx < w; ++imx) {
                for (int imy = 0; imy < h; ++imy) {
                    double realx = getRealX1(imx);
                    double realy = getRealY1(imy);
                    double v = gfm.calcFn(realx, realy);
                    double colorweight = (v-min)/(max-min);
                    int cidx1 = (int) Math.floor(colorweight * ms.n);
                    int cidx2 = Math.min(zcolors.length-1, cidx1 + 1);
                    int color1 = zcolors[cidx1].getRGB();
                    int color2 = zcolors[cidx2].getRGB();

                    int resultcolor = 0xff000000;
                    for (int mask = 0; mask <= 16; mask += 8) {
                        int c1 = (color1 >> mask) & 0xff;
                        int c2 = (color2 >> mask) & 0xff;
                        double d2 = colorweight * ms.n - cidx1;
                        double d1 = cidx2 - colorweight * ms.n;
                        int cres = (int)Math.round((d1 * c1 + d2*c2) / (d1+d2));
                        if (cres < 0)
                            cres = 0;
                        if (cres > 255)
                            cres = 255;
                        resultcolor |= (cres << mask);
                    }

                    bi.setRGB(imx, imy, resultcolor);
                }
            }
        }

        if (drawIsolines) {
            g2.setPaint(viewSettings.isoColor);
            //System.out.println("lines: " + isoarrLen);
            for (int i = 0; i < isoarrLen; ++i) {
                g2.drawLine(isoarr[i][0], isoarr[i][1], isoarr[i][2], isoarr[i][3]);
            }
        }

        if (drawXGrid) {
            for (int i = 0; i < ms.k; ++i) {
                g2.setPaint(gridColor);
                int x = (int) Math.round(i * img_xstep);
                g2.drawLine(x, 0, x, h);
                if (drawXLabels && i % tickMod == 0) {
                    g2.setColor(labelColor);
                    double tv = Math.round((i * gfm.getXstep() + ms.a) * 100.0) / 100.0;
                    g2.drawString(String.valueOf(tv), x - 4, 30);
                }
            }
        }
        if (drawYGrid) {
            for (int i = 0; i < ms.m; ++i) {
                g2.setPaint(gridColor);
                int y =  (int) Math.round(i * img_ystep);
                g2.drawLine(0, y, w, y);
                if (drawYLabels && i % tickMod == 0) {
                    g2.setColor(labelColor);
                    double tv = Math.round((i * gfm.getYstep() + ms.c) * 100.0) / 100.0;
                    g2.drawString(String.valueOf(tv), 5, y);
                }
            }

        }

        SwingUtilities.invokeLater(() -> repaint());
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);
        if (gfm == null)
            return;

        Graphics2D g2 = (Graphics2D) g;
        g2.drawImage(bi, null, null);
        g2.drawRect(0, 0, w - 1, h - 1);
    }

    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof GridFunctionModel) {
            this.gfm = (GridFunctionModel) arg;
            int newsz = gfm.getModelSettings().k * gfm.getModelSettings().m * gfm.getModelSettings().n * 2;
            if (isoarr == null || isoarr.length < newsz)
                isoarr = new int[newsz][4];
            forceRepaint();
        }
    }

    private boolean userIsoline = false;
    private double userIsoRx, userIsoRy;

    public void setUserIsoline(double rx, double ry) {
        userIsoline = true;
        userIsoRx = rx;
        userIsoRy = ry;
        forceRepaint();
    }

    public void unsetUserIsoline() {
        userIsoline = false;
        forceRepaint();
    }
}
