package ru.vlitomsk.raytr.geom.base;

/**
 * Created by vas on 23.05.2016.
 */
public class Util {
    private static final double eps = 1e-1;
    public static boolean atSegment(Vec pt, Vec a, Vec b, double totLen) {
        return Math.abs(pt.add(a, -1).norm3d() + pt.add(b, -1).norm3d() - totLen) < eps;
    }

    public static boolean atSegment(Vec pt, Vec a, Vec b) {
        return Math.abs(pt.add(a, -1).norm3d() + pt.add(b, -1).norm3d() - a.add(b, -1).norm3d()) < eps;
    }
}
