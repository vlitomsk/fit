package ru.nsu.ccfit.litvinov.cg.isolines;

import java.util.ArrayList;
import java.util.Observable;

public class GridFunctionModel extends Observable {
    public double calcFn(double realX1, double realY1) {
        return function.f(realX1, realY1);
    }

    @FunctionalInterface
    private interface GridConsumerIJXY {
        public void consume(int i, int j, double x, double y);
    }

    @FunctionalInterface
    private interface GridConsumerIJ {
        public void consume(int i, int j);
    }

    private XYFunction function;
    private ModelSettings modelSettings;
    private double[][] gridFunc;

    public double getXstep() {
        return xstep;
    }

    public double getYstep() {
        return ystep;
    }

    private double xstep, ystep;
    private double rev_xstep, rev_ystep;
    private double rev_xystep;

    public GridFunctionModel(XYFunction function, ModelSettings modelSettings) {
        this.function = function;
        this.modelSettings = modelSettings;
    }

    public void newSettings(ModelSettings res) {
        res.applyToSettings(modelSettings);
        gridFunc = new double[modelSettings.k+1][modelSettings.m+1];
        xstep = (modelSettings.b - modelSettings.a) / modelSettings.k;
        ystep = (modelSettings.d - modelSettings.c) / modelSettings.m;
        rev_xstep = 1 / xstep;
        rev_ystep = 1 / ystep;
        rev_xystep = 1 / (xstep * ystep);
        gridTraverse((i, j, x, y) -> gridFunc[i][j] = function.f(x, y));
        setChanged();
        notifyObservers(this);
    }

    private void gridTraverse(GridConsumerIJXY cons) {
        for (int i = 0; i < modelSettings.k+1; ++i) {
            for (int j = 0; j < modelSettings.m+1; ++j) {
                cons.consume(i, j, modelSettings.a + i * xstep, modelSettings.c + j * ystep);
            }
        }
    }

    private void gridTraverse(GridConsumerIJ cons) {
        for (int i = 0; i < modelSettings.k+1; ++i) {
            for (int j = 0; j < modelSettings.m+1; ++j) {
                cons.consume(i, j);
            }
        }
    }

    public double getMin() {
        final double[] min = {Double.MAX_VALUE};
        gridTraverse((i,j) -> {
            min[0] = Math.min(min[0], gridFunc[i][j]);
        });
        return min[0];
    }

    public double getMax() {
        final double[] max = {Double.MIN_VALUE};
        gridTraverse((i,j) -> {
            max[0] = Math.max(max[0], gridFunc[i][j]);
        });
        return max[0];
    }


    private static int boundInt(int x, int a, int b) {
        if (x < a)
            return a;
        if (x > b)
            return b;
        return x;
    }

    public ModelSettings getModelSettings() {
        return modelSettings;
    }

    private static double linInterp(double f1, double f2, double x1, double rev_step, double x) {
        return f1 + (f2 - f1) * (x - x1) * rev_step;
    }

    private double blinInterp(double q11, double q21, double q12, double q22, double x1, double x2, double y1, double y2, double x, double y) {
        return rev_xystep * ((q11 * (x2 - x) + q21 * (x - x1)) * (y2 - y) +
                             (q12 * (x2 - x) + q22 * (x - x1)) * (y - y1));
    }

    public double getInterpolated(double x, double y) {
        int i = (int) Math.floor((x - modelSettings.a) / xstep);
        int j = (int) Math.floor((y - modelSettings.c) / ystep);
        i = boundInt(i, 0, modelSettings.k - 1);
        j = boundInt(j, 0, modelSettings.m - 1);

        if (i == modelSettings.k - 1 && j == modelSettings.m - 1) {
            return gridFunc[i][j];
        }
        if (i == modelSettings.k - 1) {
            return linInterp(gridFunc[i][j], gridFunc[i][j + 1], j * ystep, rev_ystep, y - modelSettings.c);
        }
        if (j == modelSettings.m - 1) {
            return linInterp(gridFunc[i][j], gridFunc[i + 1][j], i * xstep, rev_xstep, x - modelSettings.a);
        }
        return blinInterp(gridFunc[i][j], gridFunc[i + 1][j], gridFunc[i][j + 1], gridFunc[i + 1][j + 1],
                i * xstep, (i + 1) * xstep, j * ystep, (j + 1) * ystep, x - modelSettings.a, y - modelSettings.c);
    }

    public double get(int i, int j) {
        return gridFunc[i][j];
    }

    public double getMid(int i, int j) {
        return function.f(i * xstep + modelSettings.a + xstep/2, j * ystep + modelSettings.c+ystep/2);
    }

    private double userz;

    public double getUserz() {
        return userz;
    }

    public ArrayList<Double> getZs(double userx, double usery) {
        userz = function.f(userx, usery);
        ArrayList<Double> zs = new ArrayList<>();
        for (int i = 0; i < modelSettings.n; ++i) {
            double zplane = (i + 1) * (getMax() - getMin()) / (modelSettings.n )+ getMin();
            double nzplane = (i + 2) * (getMax() - getMin()) / (modelSettings.n)+ getMin();

            if (i == 0 && userz < zplane) {
                zs.add(userz);
            }
            zs.add(zplane);
            if (userz >= zplane && userz <= nzplane) {
                zs.add(userz);
            }
        }
        return zs;
    }

    public ArrayList<Double> getZs() {
        ArrayList<Double> zs = new ArrayList<>();
        for (int i = 0; i < modelSettings.n; ++i) {
            double zplane = (i + 1) * (getMax() - getMin()) / modelSettings.n + getMin();
            zs.add(zplane);
        }
        return zs;
    }
}
