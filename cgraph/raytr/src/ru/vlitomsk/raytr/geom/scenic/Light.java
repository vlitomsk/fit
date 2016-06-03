package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.transform.Transform;

import java.awt.*;

/**
 * Created by vas on 23.05.2016.
 */
public class Light {
    public Vec pos;
    public Color color;

    public Light(Vec pos, Color color) {
        this.pos = pos;
        this.color = color;
    }

    @Override
    public Light clone() {
        return new Light(pos.clone(), color);
    }

    public Light assign(Light l) {
        pos.assign3(l.pos);
        color = l.color;
        return this;
    }

    public void ptransform(Transform t) {
        t.ptransform(pos);
    }
}
