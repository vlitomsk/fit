package ru.vlitomsk.raytr.geom.base;

import static java.lang.Math.*;

public class Vec {
    public static final int N = 4;
    public double[] comp;
    private double[] comp2 = new double[N];

    public Vec() {
        comp = new double[]{0,0,0,0};
    }

    public Vec(double x, double y) {
        comp = new double[]{x,y,0,1};
    }

    public Vec(double x, double y, double z) {
        comp = new double[]{x,y,z,1};
    }

    public Vec(double x, double y, double z, double w) {
        comp = new double[]{x,y,z,w};
    }

    public double x() {
        return comp[0];
    }

    public double y() {
        return comp[1];
    }

    public double z() {
        return comp[2];
    }

    public double w() {
        return comp[3];
    }

    public Vec ppow3(double gamma) {
        for (int i=0; i<3;++i)
            comp[i] = pow(comp[i], gamma);
        return this;
    }

    public Vec(double[] coord) {
        comp = new double[N];
        System.arraycopy(coord, 0, comp, 0, N);
    }

    public Vec assign(int N, Vec v) {
        for (int i = 0; i < 4; ++i)
            comp[i] = v.comp[i];
        return this;
    }

    public Vec assign3(Vec v) { return assign(3,v);}
    public Vec assign4(Vec v) { return assign(4,v);}

    private void swapComp() {
        double[] tmp = comp2;
        comp2 = comp;
        comp = tmp;
    }

    public Vec clone() {
        Vec v = new Vec();
        System.arraycopy(comp, 0, v.comp, 0, N);
        return v;
    }

    private static Vec add(Vec retv, Vec v, double coef) {
        for (int i = 0; i < N-1; ++i) {
            retv.comp[i] += coef * v.comp[i];
        }
        return retv;
    }

    public Vec padd(Vec v) {
        return add(this, v, 1.0);
    }

    public Vec padd(Vec v, double coef) { return add(this, v, coef); }

    public Vec add(Vec v, double coef) { return add(clone(), v, coef); }

    private static Vec coef(int N, Vec retv, double coef) {
        for (int i = 0; i < N; ++i) {
            retv.comp[i] *= coef;
        }
        return retv;
    }

    public Vec pcoef3(double coef) {
        return coef(3, this, coef);
    }
    public Vec pcoef4(double coef) { return coef(4, this, coef); }

    public double dot(int N, Vec v) {
        double d = 0;
        for (int i = 0; i < N; ++i) {
            d += comp[i] * v.comp[i];
        }
        return d;
    }

    private static Vec linop(Vec retv, double[][] op) {
        for (int i = 0; i < N; ++i) {
            retv.comp2[i] = 0;
            for (int j = 0; j < N; ++j) {
                retv.comp2[i] += retv.comp[j] * op[i][j];
            }
        }
        retv.swapComp();
        return retv;
    }

    public Vec linop(double[][] op) {
        return linop(clone(), op);
    }

    public Vec plinop(double[][] op) {
        return linop(this, op);
    }

    public double norm3d() {
        return sqrt(sqrNorm3d());
    }

    public double sqrNorm3d() {
        double s = 0;
        for (int i = 0; i < 3; ++i)
            s += comp[i] * comp[i];
        return s;
    }

    public Vec pnormalize3d() {
        coef(3, this, 1.0 / this.norm3d());
        comp[3] = 1;
        return this;
    }

    public Vec normalize3d() {
        Vec cl = clone();
        coef(3, cl, 1.0 / cl.norm3d());
        cl.comp[3] = 1;
        return cl;
    }

    private static Vec cross3d(Vec retv, Vec b) {
        retv.comp2[0] = retv.comp[1] * b.comp[2] - b.comp[1] * retv.comp[2];
        retv.comp2[1] = b.comp[0] * retv.comp[2] - retv.comp[0] * b.comp[2];
        retv.comp2[2] = retv.comp[0] * b.comp[1] - b.comp[0] * retv.comp[1];
        retv.comp2[3] = 1;
        retv.swapComp();
        return retv;
    }

    public Vec pcross3d(Vec b) {
        return cross3d(this, b);
    }

    public Vec cross3d(Vec b) {
        return cross3d(clone(), b);
    }

    private Vec compwiseMul(int N, Vec v1, Vec v2) {
        for (int i =0; i < N; ++i) {
            v1.comp[i] *= v2.comp[i];
        }
        return v1;
    }

    public Vec pcompwiseMul3(Vec v) { return compwiseMul(3, this, v); }

    public Vec psatur3(double a, double b) {
        for (int i = 0; i < 4; ++i) {
            if (comp[i] < a)
                comp[i] = a;
            if (comp[i] > b)
                comp[i] = b;
        }
        return this;
    }

    @Override
    public String toString() {
        return "(" + comp[0] + ", " + comp[1] + ", " + comp[2] + ", " + comp[3] + ")";
    }
}
