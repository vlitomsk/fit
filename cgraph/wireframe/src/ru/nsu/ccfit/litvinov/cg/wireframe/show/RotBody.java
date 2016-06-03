package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class RotBody implements Sampleable {
    private Segment[] generatrix;

    public RotBody(Segment[] generatrix) {
        this.generatrix = generatrix;
    }

    @Override
    public Segment[] sample(int segments) {
        final double d_phi = 2.0 * Math.PI / segments;
        int reslen = segments * generatrix.length;
        Segment[] result = new Segment[reslen];
        for (int i = 0; i < segments; ++i) {
            double ang = d_phi * i;
            double sin = Math.sin(ang);
            double cos = Math.cos(ang);
            for (int j = 0; j < generatrix.length; ++j) {
                result[i * generatrix.length + j] = generatrix[j].clone();
                result[i * generatrix.length + j].rotX(sin, cos);
            }
        }
        return result;
    }
}
