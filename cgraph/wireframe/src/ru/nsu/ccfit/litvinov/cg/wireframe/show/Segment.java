package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class Segment {
    public Vec v1, v2;

    public Segment(Vec v1, Vec v2) {
        this.v1 = v1;
        this.v2 = v2;
    }

    public Segment(double x1, double y1, double x2, double y2) {
        this.v1 = new Vec(x1, y1);
        this.v2 = new Vec(x2, y2);
    }

    public double x1() {
        return v1.coord[0];
    }

    public double y1() {
        return v1.coord[1];
    }

    public double x2() {
        return v2.coord[0];
    }

    public double y2() {
        return v2.coord[1];
    }

    public void rotX(double sin, double cos) {
        v1.rotX(sin, cos);
        v2.rotX(sin, cos);
    }

    public Segment clone() {
        return new Segment(v1.clone(), v2.clone());
    }
}
