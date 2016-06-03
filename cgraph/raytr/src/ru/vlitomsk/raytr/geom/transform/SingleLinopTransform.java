package ru.vlitomsk.raytr.geom.transform;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 16.05.2016.
 */
public class SingleLinopTransform implements LinopTransform {
    private double[][] mat;

    public SingleLinopTransform() {}

    public SingleLinopTransform(double[][] mat) {
        this.mat = mat;
    }

    @Override
    public double[][] getMat() {
        return mat;
    }

    public void setMat(double[][] mat) {
        this.mat = mat;
    }

    @Override
    public Vec ptransform(Vec v) {
        return v.plinop(mat);
    }
}
