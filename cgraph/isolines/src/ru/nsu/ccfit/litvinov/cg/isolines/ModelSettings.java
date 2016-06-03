package ru.nsu.ccfit.litvinov.cg.isolines;

import java.util.Observable;

/**
 * Created by v on 4/4/16.
 */
public class ModelSettings {
    public int k, m; // grid size k cells * m cells
    public double a, b, c, d; // dom x , dom y
    public int n; // isolines count

    public void applyToSettings(ModelSettings s) {
        s.k = k;
        s.m = m;
        s.a = a;
        s.b = b;
        s.c = c;
        s.d = d;
    }

    public ModelSettings() {}

    public ModelSettings(int k, int m, int n, double a, double b, double c, double d) {
        this.k = k;
        this.m = m;
        this.n = n;
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
    }
}
