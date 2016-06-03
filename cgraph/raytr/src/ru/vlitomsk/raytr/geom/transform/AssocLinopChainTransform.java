package ru.vlitomsk.raytr.geom.transform;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 16.05.2016.
 */
public class AssocLinopChainTransform implements LinopTransform {
    private LinopTransform[] transforms;
    private int changeIdx = -1;

    private static final int BEFORE = 0, CHGMAT = 1, AFTER = 2; // idxs for partmats
    private double[][][] partmats = new double[3][][];

    public AssocLinopChainTransform(LinopTransform[] transforms) {
        this.transforms = transforms;
    }

    public void willChange(LinopTransform tr) {
        for (int i = 0; i < transforms.length; ++i) {
            if (transforms[i] == tr) {
                changeIdx = i;
                computePartmats();
                return;
            }
        }
        throw new RuntimeException("Given LinopTransform not found in transforms[]");
    }

    public void willChange(int changeIdx) {
        if (changeIdx < 0 || changeIdx >= transforms.length)
            throw new RuntimeException("changeIdx < 0 || changeIdx >= transforms.length");
        this.changeIdx = changeIdx;
        computePartmats();
    }

    public void nothingWillChange() {
        changeIdx = -1;
        computePartmats();
    }

    private double[][] tmp = new double[4][4];
    private boolean inTmp = false;

    private double[][] cloneMat(double[][] orig) {
        double[][] newMat = new double[4][4];
        copyMat(orig, newMat);
        return newMat;
    }

    private void copyMat(double[][] src, double[][] dst) {
        for (int i = 0; i < 4; ++i) {
            System.arraycopy(src[i], 0, dst[i], 0, 4);
        }
    }

    private void accumulatePartmat(int pmIdx, int ifrom, int ito) {
        inTmp = false;
        for (int i = ifrom + 1; i < ito; ++i) {
            if (partmats[pmIdx] == null) {
                partmats[pmIdx] = cloneMat(transforms[i].getMat());
            } else {
                Linop.mmult(inTmp ? tmp : partmats[pmIdx], transforms[i].getMat(), inTmp ? partmats[pmIdx] : tmp);
                inTmp = !inTmp;
            }
        }
        if (inTmp) {
            copyMat(tmp, partmats[pmIdx]);
        }
    }

    private void computePartmats() {
        if (changeIdx >= 0) {
            partmats[BEFORE] = partmats[AFTER] = null;

            accumulatePartmat(BEFORE, 0, changeIdx);
            //partmats[CHGMAT] = transforms[changeIdx].getMat();
            accumulatePartmat(AFTER, changeIdx + 1, transforms.length);
        } else {
            accumulatePartmat(BEFORE, 0, transforms.length);
        }
    }

    @Override
    public double[][] getMat() {
        throw new RuntimeException("hey dude u don't need dis method!");
//        if (changeIdx >= 0) {
//
//        } else {
//            return partmats[BEFORE];
//        }
    }

    @Override
    public Vec ptransform(Vec v) {
        if (partmats[BEFORE] != null) {
            v.plinop(partmats[BEFORE]);
        }
        if (changeIdx >= 0) {
            //v.plinop(transforms[changeIdx].getMat());
            transforms[changeIdx].ptransform(v);
            if (partmats[AFTER] != null)
                v.plinop(partmats[AFTER]);
        }
        return v;
    }
}
