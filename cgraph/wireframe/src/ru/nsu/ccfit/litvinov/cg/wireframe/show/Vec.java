package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class Vec {
    public double [] coord;
    public double [] coord1 = new double[4];

    public double x() {
        return coord[0];
    }

    public double y() {
        return coord[1];
    }

    public double z() {
        return coord[2];
    }

    public double w() {
        return coord[3];
    }

    public Vec(double [] coord) {
        this.coord = coord;
    }

    private void swap() {
        double[] tmp = coord1;
        coord1 = coord;
        coord = tmp;
    }

    public Vec(double x, double y) {
        coord = new double[] {x, y, 0, 1};
    }

    public Vec(double x, double y, double z) {
        coord = new double[] {x, y, z, 1};
    }

    static double[][] mat_rotX = {
            {1, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 1}
    };
    public void rotX(double sin, double cos) {
        mat_rotX[1][1] = mat_rotX[2][2] = cos;
        mat_rotX[1][2] = -sin;
        mat_rotX[2][1] = sin;

        lmmult(mat_rotX);
    }

    public Vec lmmult(double[][] mat) {
        for (int i = 0; i < 4; ++i) {
            coord1[i] = 0;
            for (int j = 0; j < 4; ++j) {
                coord1[i] += coord[j] * mat[i][j];
            }
        }
        swap();
        return this;
    }

    public Vec sub(Vec v) {
        for (int i = 0; i < 4; ++i) {
            coord[i] -= v.coord[i];
        }
        return this;
    }

    public Vec normalize() {
        double sum = 0;
        for (int i = 0; i < 4; ++i) {
            sum += coord[i] * coord[i];
        }
        sum = 1.0 / Math.sqrt(sum);
        for (int i = 0; i < 4; ++i) {
            coord[i] *= sum;
        }
        return this;
    }

    public Vec cross3(Vec v) {
        coord1[3] = 1;
        coord1[2] = x() * v.y() - v.x() * y();
        coord1[1] = x() * v.z() - v.x() * z();
        coord1[0] = y() * v.z() - v.y() * z();
        swap();
        return this;
    }

    public Vec clone() {
        double [] c = new double[coord.length];
        for (int i = 0; i < coord.length; ++i) {
            c[i] = coord[i];
        }
        return new Vec(c);
    }
}
