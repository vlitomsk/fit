package ru.nsu.ccfit.litvinov.cg.wireframe.show;

/**
 * Created by v on 4/19/16.
 */
public class Projector {
    private Vec pCam = new Vec(-10, 0, 0);
    private Vec pView = new Vec(10, 0, 0);
    private Vec vUp = new Vec(0, 1, 0);

    private double zN = 10;
    private double zF = 15;
    private double sW = 20;
    private double sH = 40;

    private double[][] mCamLeft() {
        Vec k = pCam.clone().sub(pView).normalize();
        Vec i = vUp.clone().cross3(k).normalize();
        Vec j = k.clone().cross3(i).normalize();

        return new double[][] {
                {i.x(), i.y(), i.z(), 0},
                {j.x(), j.y(), j.z(), 0},
                {k.x(), k.y(), k.z(), 0},
                {0, 0, 0, 1}
        };
    }

    private double[][] mCamRight() {
        return new double[][] {
                {1, 0, 0, -pCam.x()},
                {0, 1, 0, -pCam.y()},
                {0, 0, 1, -pCam.z()},
                {0, 0, 0, 1}
        };
    }

    public double[][] mCam() {
        return mmult_nn(mCamLeft(), mCamRight(), 4);
    }

    public double[][] assocMat() {
        return mmult_nn(mProj(), mCam(), 4);
    }

    public double[][] mProj() {
        return new double[][] {
                {(2 / sW) * zN, 0,              0,                0},
                {0,             (2 / sH) * zN,  0,                0},
                {0,             0,              (zF) / (zF - zN), (-zN * zF) / (zF - zN) },
                {0,             0,              1,                0}
        };
    }

    private double [][] mmult_nn(double [][] a, double [][] b, int n) {
        double[][] c = new double[n][n];
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                for (int k = 0; k < n; ++k) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }

        return c;
    }
}
