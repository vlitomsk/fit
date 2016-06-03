package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Inters;
import ru.vlitomsk.raytr.geom.base.Segment;
import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.render.intersect.BoundedObject;
import ru.vlitomsk.raytr.geom.transform.Transform;

public abstract class SceneObject implements BoundedObject {
    protected Segment[] segments = new Segment[]{};
    protected OpticParams opticParams = new OpticParams();
    public Segment[] getSegments() {
        return segments;
    }

    public OpticParams getOpticParams() {
        return opticParams;
    }

    public void setOpticParams(OpticParams opticParams) {
        this.opticParams = opticParams;
    }

    // p is start of ray, d is ray direction (may be not-normalized)
    public abstract Vec rayIntersect(Inters inters, Vec p, Vec d);

    public abstract void ptransform(Transform tr);

    @Override
    public abstract SceneObject clone();

    public abstract void assign(SceneObject obj);

    public abstract boolean atEdge(Vec v);

    public abstract Vec calcNorm(Vec pt, Vec v);
}
