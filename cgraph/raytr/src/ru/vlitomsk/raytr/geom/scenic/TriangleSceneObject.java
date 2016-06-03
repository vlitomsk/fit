package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Inters;
import ru.vlitomsk.raytr.geom.base.Segment;
import ru.vlitomsk.raytr.geom.base.Util;
import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.transform.Transform;

import static java.lang.Math.max;
import static java.lang.Math.min;

/**
 * Created by vas on 17.05.2016.
 */
public class TriangleSceneObject extends SceneObject {
    private Vec p0,p1,p2;
    //private Segment[] segments;
    public TriangleSceneObject(Vec p0, Vec p1, Vec p2) {
        this.p0 = p0;
        this.p1 = p1;
        this.p2 = p2;
        segments = new Segment[]{new Segment(p0, p1), new Segment(p1, p2), new Segment(p2, p0)};
    }

    @Override
    public void ptransform(Transform tr) {
        tr.ptransform(p0);
        tr.ptransform(p1);
        tr.ptransform(p2);
        segments = new Segment[]{new Segment(p0, p1), new Segment(p1, p2), new Segment(p2, p0)};
    }

    @Override
    public void assign(SceneObject obj) {
        TriangleSceneObject t = (TriangleSceneObject) obj;
        p0.assign3(t.p0);
        p1.assign3(t.p1);
        p2.assign3(t.p2);
        segments = new Segment[]{new Segment(p0, p1), new Segment(p1, p2), new Segment(p2, p0)};
    }

    @Override
    public boolean atEdge(Vec v) {
        return Util.atSegment(v, p0, p1) || Util.atSegment(v, p1, p2) || Util.atSegment(v, p2, p0);
    }

    @Override
    public TriangleSceneObject clone() {
        TriangleSceneObject t = new TriangleSceneObject(p0.clone(), p1.clone(), p2.clone());
        t.setOpticParams(getOpticParams());
        return t;
    }

    private Vec tmpv = new Vec();
    @Override
    public Vec calcNorm(Vec pt, Vec v) {
        return v.assign3(p0).padd(p1, -1).pcross3d(tmpv.assign3(p0).padd(p2, -1)).pnormalize3d();
    }

    @Override
    public Vec rayIntersect(Inters inters, Vec p, Vec d) {
        return inters.rayTr(p, d, p0, p1, p2);
    }

    @Override
    public double minX() {
        return min(p0.x(), min(p1.x(), p2.x()));
    }

    @Override
    public double minY() {
        return min(p0.y(), min(p1.y(), p2.y()));
    }

    @Override
    public double minZ() {
        return min(p0.z(), min(p1.z(), p2.z()));
    }

    @Override
    public double maxX() {
        return max(p0.x(), max(p1.x(), p2.x()));
    }

    @Override
    public double maxY() {
        return max(p0.y(), max(p1.y(), p2.y()));
    }

    @Override
    public double maxZ() {
        return max(p0.z(), max(p1.z(), p2.z()));
    }
}
