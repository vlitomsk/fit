package ru.vlitomsk.raytr.geom.render;

import ru.vlitomsk.raytr.geom.base.Vec;

import java.awt.*;

public class ViewportCameraSettings extends CameraSettings {
    private Color bgColor = Color.black, lineColor = Color.black;

    private double imW = 600;
    private double imH = 600;
    private double ratio = imH/imW;
    //private double ratio = 1.0;

    protected Vec center = new Vec(imW * 0.5, imH * 0.5);
    protected double
            xmult = Sw*Sw*0.5/imW, // для переведения точки экрана в направление луча
            ymult = Sh*Sh*0.5/imH;


    public Vec screenToDir(Vec dir, int i, int j) {
        dir.comp[0] = xmult*(i - imW * 0.5);
        dir.comp[1] = ymult*((imH-j) - imH * 0.5);
        dir.comp[2] = zf;
        dir.comp[3] = 0;

        return dir.pnormalize3d();
    }

    public void setImWH(int w, int h) {
        imW = w;
        imH = h;
        ratio = imH / imW;
        xmult = Sw*Sw*0.5/imW;
        ymult = Sh*Sh*0.5/imH;
        center.comp[0] = imW * 0.5;
        center.comp[1] = imH * 0.5;
    }

    public double getImH() {
        return imH;
    }

    public double getImW() {
        return imW;
    }

    public double getRatio() {
        return ratio;
    }

    public double getXmult() {
        return xmult;
    }

    public double getYmult() {
        return ymult;
    }

    public Vec getCenter() {
        return center;
    }

    public void setSw(double sw) {
        Sw = sw;
        Sh = Sw*ratio;
        xmult = Sw*Sw*0.5/imW;
    }

    public void setSh(double sh) {
        Sh = sh;
        ymult = Sh*Sh*0.5/imH;
    }

    public Color getBgColor() {
        return bgColor;
    }

    public Color getLineColor() {
        return lineColor;
    }

    public void setBgColor(Color bgColor) {
        this.bgColor = bgColor;
    }

    public void setLineColor(Color lineColor) {
        this.lineColor = lineColor;
    }
}
