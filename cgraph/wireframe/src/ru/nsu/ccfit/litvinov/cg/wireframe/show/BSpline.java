package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class BSpline implements Sampleable {
    private double[][] knots;

    public BSpline(double[][] knots) {
        this.knots = knots;
        if (knots.length < 4) {
            throw new RuntimeException("Too few knots (knots.length < 4)");
        }
    }

    @Override
    public Segment[] sample(int segments) {
        segments = Math.min(knots.length - 1, segments);
        Segment[] result = new Segment[segments];
        for (int i = 0; i < segments; ++i) {
            result[i] = new Segment(knots[i][0], knots[i][1], knots[i+1][0], knots[i+1][1]);
        }
        return result;
    }
}
