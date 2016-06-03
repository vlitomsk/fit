package ru.vlitomsk.raytr.geom.transform;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 16.05.2016.
 */
public class Linop {
    public double[][] trans(double tx, double ty, double tz) {
        double[][] trMat = new double[4][4];
        for (int i = 0; i < 4; ++i)
            trMat[i][i] = 1.0;
        trMat[0][3] = tx;
        trMat[1][3] = ty;
        trMat[2][3] = tz;
        return trMat;
    }

    public double[][] scale(double sx, double sy, double sz) {
        double[][] scMat = new double[4][4];
        scMat[0][0] = sx;
        scMat[1][1] = sy;
        scMat[2][2] = sz;
        scMat[3][3] = 1;
        return scMat;
    }

    private double[][] rot(double cos, double sin, int ri1, int ri2, int fi) {
        double[][] rotMat = new double[4][4];
        rotMat[3][3] = 1;
        rotMat[fi][fi] = 1;
        rotMat[ri1][ri1] = rotMat[ri2][ri2] = cos;
        rotMat[ri1][ri2] = -sin;
        rotMat[ri2][ri1] = sin;
        return rotMat;
    }

    public double[][] rotX(double ang) {
        return rotX(Math.cos(ang), Math.sin(ang));
    }

    public double[][] rotY(double ang) {
        return rotY(Math.cos(ang), Math.sin(ang));
    }

    public double[][] rotZ(double ang) {
        return rotZ(Math.cos(ang), Math.sin(ang));
    }

    public double[][] rotX(double cos, double sin) {
        return rot(cos, sin, 1, 2, 0);
    }

    public double[][] rotY(double cos, double sin) {
        return rot(cos, sin, 0, 2, 1);
    }

    public double[][] rotZ(double cos, double sin) {
        return rot(cos, sin, 0, 1, 2);
    }

    public double[][] ortProj(double boxW, double boxH, double zNear, double zFar) {
        double[][] projMat = new double[4][4];
        projMat[0][0] = 1.0 / boxW;
        projMat[1][1] = 1.0 / boxH;
        projMat[2][2] = -2.0 / (zFar - zNear);
        projMat[2][3] = -(zFar + zNear) / (zFar - zNear);
        projMat[3][2] = 0;
        projMat[3][3] = 1;
        return projMat;
    }

//    public double[][] perspProj(double fovX, double fovY, double zNear, double zFar) {
//        projMat[0][0] = Math.atan(fovX / 2.0);
//        projMat[1][1] = Math.atan(fovY / 2.0);
//        projMat[2][2] = -(zFar + zNear) / (zFar - zNear);
//        projMat[2][3] = -2.0 * zNear * zFar / (zFar - zNear);
//        projMat[3][2] = -1;
//        projMat[3][3] = 0;
//        return projMat;
//    }

    public double[][] perspProj(double Sw, double Sh, double zf, double zb) {
        double[][] projMat = new double[4][4];
        projMat[0][0] = 2.0 * zf / Sw;
        projMat[1][1] = 2.0 * zf / Sh;
        //projMat[0][0] = projMat[1][1] = zf;
        projMat[2][2] = zb / (zb - zf);
        projMat[2][3] = -zf * zb / (zb - zf);
        projMat[3][2] = 1;
        projMat[3][3] = 0;
        return projMat;
    }

    private double[][] crm = new double[][] {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 1}
    };

    public double[][] camRotMat(Vec pos, Vec view, Vec up) {
        Vec k = pos.add(view, -1.0).pnormalize3d();
        Vec i = up.cross3d(k).pnormalize3d();
        Vec j = k.cross3d(i).pnormalize3d();
        crm[0][0] = i.x(); crm[0][1] = i.y(); crm[0][2] = i.z();
        crm[1][0] = j.x(); crm[1][1] = j.y(); crm[1][2] = j.z();
        crm[2][0] = k.x(); crm[2][1] = k.y(); crm[2][2] = k.z();
        return crm;
    }

    public double[][] camTransMat(Vec pos) {
        return trans(-pos.x(), -pos.y(), -pos.z());
    }

    private static final int N = 4;
    public static double[][] mmult(double[][] a, double[][] b, double[][] c) {
        //double[][] c = new double[N][N];

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                c[i][j] = 0;
                for (int k = 0; k < N; ++k) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return c;
    }
}
