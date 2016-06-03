package ru.vlitomsk.raytr.geom.render;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 23.05.2016.
 */

public class CameraSettings {
    protected double Sw = 4, Sh = 4, zf = -4.74, zb = -90;
    protected Vec camPos = new Vec(-1.06, -5.7, 10);
    protected Vec camView = new Vec(5.96, 9.39, -1.45);
    protected Vec camUp = new Vec(-3.2, -7.09, 6.35);

    public CameraSettings() {
        recalcIJK();
    }

    public CameraSettings(double sw, double sh, double zf, double zb, Vec camPos, Vec camView, Vec camUp) {
        Sw = sw;
        Sh = sh;
        this.zf = zf;
        this.zb = zb;
        this.camPos = camPos;
        this.camView = camView;
        this.camUp = camUp;
        recalcIJK();
    }

    private void recalcIJK() {
        kvec.assign3(camPos).padd(camView, -1.0).pnormalize3d();
        ivec.assign3(camUp).pcross3d(kvec).pnormalize3d();
        jvec.assign3(kvec).pcross3d(ivec).pnormalize3d();
    }

    private Vec kvec = new Vec();
    private Vec jvec = new Vec();
    private Vec ivec = new Vec();

    public Vec getK() {
        return kvec;
    }

    public Vec getI() {
        return ivec;
    }

    public Vec getJ() {
        return jvec;
    }

    public double getSw() {
        return Sw;
    }

    public double getSh() {
        return Sh;
    }

    public double getZf() {
        return zf;
    }

    public double getZb() {
        return zb;
    }

    public Vec getCamPos() {
        return camPos;
    }

    public Vec getCamView() {
        return camView;
    }

    public Vec getCamUp() {
        return camUp;
    }

    public void setZf(double zf) {
        this.zf = zf;
    }

    public void setZb(double zb) {
        this.zb = zb;
    }

    public void setCamPos(Vec camPos) {
        this.camPos = camPos;
        recalcIJK();
    }

    public void setCamView(Vec camView) {
        this.camView = camView;
        recalcIJK();
    }

    public void setCamUp(Vec camUp) {
        this.camUp = camUp;
        recalcIJK();
    }
}
