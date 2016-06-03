package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Vec;

import java.awt.*;

/**
 * Created by vas on 23.05.2016.
 */
public class OpticParams {
   // public Color color;
    public Vec coefDiffuse, coefMirror;
    public double power; // phong

    public OpticParams() {
       //color = Color.blue;
        coefDiffuse = new Vec(0.4, 0.4, 0.4, 0);
        coefMirror = new Vec(1, 1, 1, 0);
        power = 0.0;
    }

    public OpticParams(Color color, Vec coefDiffuse, Vec coefMirror, double power) {
        //this.color = color;
        this.coefDiffuse = coefDiffuse;
        this.coefMirror = coefMirror;
        this.power = power;
    }
}
