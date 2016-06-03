package ru.vlitomsk.raytr.geom.transform;

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * Created by vas on 16.05.2016.
 */
@FunctionalInterface
public interface Transform {
    public Vec ptransform(Vec v);
}
