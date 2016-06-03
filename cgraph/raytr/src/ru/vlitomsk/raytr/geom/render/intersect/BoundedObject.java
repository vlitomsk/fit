package ru.vlitomsk.raytr.geom.render.intersect;

/**
 * Created by vas on 30.05.2016.
 */
public interface BoundedObject {
    public double minX();
    public double minY();
    public double minZ();
    public double maxX();
    public double maxY();
    public double maxZ();
}
