package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class Domain {
    private double domx1, domy1, domx2, domy2; // dom of generatrix
    private double sdomx1, sdomy1;
    private double mapCoef, mapCoef_1;
    private int w, h;

    public Domain(double domx1, double domy1, double domx2, double domy2) {
        this.domx1 = Math.min(domx1, domx2);
        this.domx2 = Math.max(domx1, domx2);
        this.domy1 = Math.min(domy1, domy2);
        this.domy2 = Math.min(domy1, domy2);
    }

    public void sizeChanged(int w, int h) {
        this.w = w;
        this.h = h;
        double domx = domx2 - domx1;
        double domy = domy2 - domy1;
        if (domx * h >= domy * w) {
            mapCoef = w / domx;
            sdomx1 = domx1;
            sdomy1 = (domy1 + domy2) * 0.5 - h * mapCoef_1 * 0.5;
        } else {
            mapCoef = h / domy;
            sdomx1 = (domx1 + domx2) * 0.5 - w * mapCoef_1 * 0.5;
            sdomy1 = domy1;
        }
        mapCoef_1 = 1.0 / mapCoef;
    }

    /// dom x -> 0..width
    public int mapx(double x) {
        return (int) Math.round((x - sdomx1) * mapCoef);
    }

    /// dom y -> 0..height
    public int mapy(double y) {
        return h - 1 - (int) Math.round((y - sdomy1) * mapCoef);
    }

    /// 0..width -> dom x
    public double unmapx(int x) {
        return ((double)x) * mapCoef_1 + sdomx1;
    }

    /// 0..height -> dom y
    public double unmapy(int y) {
        return ((double)(y - h + 1)) * mapCoef_1 + sdomy1;
    }

}
