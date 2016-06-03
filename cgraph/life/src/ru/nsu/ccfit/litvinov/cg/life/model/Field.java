package ru.nsu.ccfit.litvinov.cg.life.model;

import ru.nsu.ccfit.litvinov.cg.life.msgs.FieldUpdate;

import java.util.Observable;
import java.util.Observer;

/**
 * Created by vas on 12.02.16.
 */
public class Field extends Observable {
    private int mSize, nSize;
    private boolean[][] fldOld;
    private boolean[][] fldNew;
    private double[][] impMat;
    private Rules rules;

    public static final boolean DEAD = false;

    public int getmSize() {
        return mSize;
    }

    public int getnSize() {
        return nSize;
    }

    public static final boolean ALIVE = true;

    boolean needRecalc = false;
    public Field(int mSize, int nSize, Rules rules) {
        this.rules = rules;
        this.mSize = mSize;
        this.nSize = nSize;
        fldOld = new boolean[mSize][nSize];
        fldNew = new boolean[mSize][nSize];
        impMat = new double[mSize][nSize];
        needRecalc = true;
        recalcImpact();
    }

    public Field(boolean[][] fld, Rules rules) {
        this.rules = rules;
        mSize = fld.length;
        nSize = fld[0].length;
        for (int i = 1; i < mSize; ++i) {
            if (fld[i].length != nSize)
                throw new RuntimeException("fld has to be NxM matrix");
        }
        fldOld = fld;
        fldNew = new boolean[mSize][nSize];
        impMat = new double[mSize][nSize];
        needRecalc = true;
        recalcImpact();
    }

    public void setRules(Rules rules) {
        this.rules = rules;
    }

    private void swap() {
        boolean[][] t = fldOld;
        fldOld = fldNew;
        fldNew = t;
    }

    private boolean vecAvail(Vec v) {
        return v.y >= 0 && v.y < nSize && v.x >= 0 && v.x < mSize;
    }

    private boolean isAlive(Vec v) {
        return fldOld[v.x][v.y] == ALIVE;
    }

    Vec iter = new Vec();
    Vec reliter = new Vec();

    private void recalcImpact() {
        for (iter.x = 0; iter.x < mSize; ++iter.x) {
            for (iter.y = 0; iter.y < nSize; ++iter.y) {
                double imp = 0;
                for (int i = 0; i < rules.neighborsRel.length; ++i) {
                    Vec neighRel = rules.neighborsRel[i];
                    reliter.set(neighRel);
                    reliter.addPlace(iter);

                    if (Math.abs(neighRel.y) % 2 == 1) {
                        if (iter.y % 2 == 0 && neighRel.x > 0) --reliter.x;
                        else if (iter.y % 2 == 1 && neighRel.x < 0) ++reliter.x;
                    }
                    if (vecAvail(reliter) && isAlive(reliter))
                        imp += rules.distImpacts[rules.dists[i]];
                }
                impMat[iter.x][iter.y] = imp;
            }
        }
    }

    public void nextStep() {
        synchronized (this) {
            if (needRecalc) {
                recalcImpact();
                needRecalc = false;
            }

            for (iter.x = 0; iter.x < mSize; ++iter.x) {
                for (iter.y = 0; iter.y < nSize; ++iter.y) {
                    final double imp = impMat[iter.x][iter.y];
                    if (rules.dieCond(imp)) {
                        fldNew[iter.x][iter.y] = DEAD;
                    } else if (rules.birthCond(imp)) {
                        fldNew[iter.x][iter.y] = ALIVE;
                    }
                }
            }
            swap();
            needRecalc = true;
            recalcImpact();
        }

        setChanged();
        notifyObservers(new FieldUpdate(this));
    }

    public void not(int i, int j) {
        synchronized (this) {
            fldOld[i][j] = !fldOld[i][j];
        }
        setChanged();
        needRecalc = true;
        recalcImpact();
        notifyObservers(new FieldUpdate(this));
    }

    public void set(int i, int j) {
        synchronized (this) {
            fldOld[i][j] = ALIVE;
        }
        setChanged();
        needRecalc = true;
        recalcImpact();
        notifyObservers(new FieldUpdate(this));
    }

    public void clear() {
        synchronized (this) {
            setChanged();
            for (int i =0; i < mSize; ++i) {
                for (int j = 0; j < nSize; ++j) {
                    fldOld[i][j] = DEAD;
                }
            }
            needRecalc = true;
            notifyObservers(new FieldUpdate(this));
        }
    }

    public boolean[][] getField() {
        synchronized (this) {
            return fldOld;
        }
    }

    public double[][] getImpMat() {
        synchronized (this) {
            return impMat;
        }
    }

    public void nSteps(int n) {
        for (; n > 0; --n) {
            nextStep();
        }
    }
}
