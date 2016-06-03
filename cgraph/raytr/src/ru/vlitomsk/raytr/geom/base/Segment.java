package ru.vlitomsk.raytr.geom.base;

public class Segment {
    public Vec[] p;

    public Segment() {
        p = new Vec[]{new Vec(), new Vec()};
    }

    public Segment(Vec a, Vec b) {
        p = new Vec[]{a,b};
    }

    public Segment(double x1, double y1, double z1, double x2, double y2, double z2) {
        p = new Vec[]{new Vec(x1,y1,z1), new Vec(x2,y2,z2)};
    }

    public Segment(Vec[] p) {
        this.p = p;
    }

    public Segment clone() {
        return new Segment(p[0].clone(), p[1].clone());
    }

    @Override
    public String toString() {
        return p[0].toString() + " <> " + p[1].toString();
    }
}
