package ru.vlitomsk.raytr.geom.transform;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 16.05.2016.
 */
public class PerspDivisionTransform implements Transform {
    @Override
    public Vec ptransform(Vec v) {
        return v.pcoef4(1.0 / v.w());
    }
}
