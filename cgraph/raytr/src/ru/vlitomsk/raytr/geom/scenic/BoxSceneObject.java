package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Inters;
import ru.vlitomsk.raytr.geom.base.Segment;
import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.transform.Transform;

/**
 * Created by vas on 17.05.2016.
 */
public class BoxSceneObject extends SceneObject {
    private double minx,miny,minz,maxx,maxy,maxz;
    Vec p000, p001, p010, p011, p100, p101, p110, p111;

    @Override
    public SceneObject clone() {
        return null;
    }

    @Override
    public void assign(SceneObject obj) {

    }

    public BoxSceneObject(double minx, double maxx, double miny, double maxy, double minz, double maxz) {
        p000 = new Vec(minx, miny, minz);
        p001 = new Vec(minx, miny, maxz);
        p010 = new Vec(minx, maxy, minz);
        p011 = new Vec(minx, maxy, maxz);
        p100 = new Vec(maxx, miny, minz);
        p101 = new Vec(maxx, miny, maxz);
        p110 = new Vec(maxx, maxy, minz);
        p111 = new Vec(maxx, maxy, maxz);
        this.minx = minx;
        this.maxx = maxx;
        this.miny = miny;
        this.maxy = maxy;
        this.minz = minz;
        this.maxz = maxz;
        segments = new Segment[]{
                new Segment(minx, miny, minz, maxx, miny, minz),
                new Segment(minx, miny, minz, minx, maxy, minz),
                new Segment(maxx, maxy, minz, minx, maxy, minz),
                new Segment(maxx, maxy, minz, maxx, miny, minz),
                new Segment(minx, miny, maxz, maxx, miny, maxz),
                new Segment(minx, miny, maxz, minx, maxy, maxz),
                new Segment(maxx, maxy, maxz, minx, maxy, maxz),
                new Segment(maxx, maxy, maxz, maxx, miny, maxz),
                new Segment(minx, miny, minz, minx, miny, maxz),
                new Segment(minx, maxy, minz, minx, maxy, maxz),
                new Segment(maxx, miny, minz, maxx, miny, maxz),
                new Segment(maxx, maxy, minz, maxx, maxy, maxz)
        };
    }

    @Override
    public void ptransform(Transform tr) {
        Vec[] pts = {p000, p001, p010, p011, p100, p101, p110, p111};
        for (Vec pt : pts) {
            tr.ptransform(pt);
        }
    }

//    @Override
//    public Vec rayIntersect(Vec p, Vec d) {
//        Vec[][] grani = {
//                {p000, p001, p011, p010},


    @Override
    public Vec calcNorm(Vec pt, Vec v) {
        return null;
    }

    @Override
    public boolean atEdge(Vec v) {
        return false;
    }
//                {p100, p101, p111, p110},
//                {p000, p001, p101, p100},
//                {p010, p011, p111, p110},
//                {p010, p000, p100, p110},
//                {p001, p011, p111, p101}
//        };
//
//        Vec minQ = null;
//        double minNorm = Double.MAX_VALUE;
//
//        for (int i = 0; i < 6; ++i) {
//            Vec[] gr = grani[i];
//            Vec q = Inters.rayQuad(p, d, gr[0], gr[1], gr[2], gr[3]);
//            if (q != null) {
//                double norm = q.norm3d();
//                if (norm < minNorm) {
//                    minNorm = norm;
//                    minQ = q;
//                }
//            }
//        }
//
//        return minQ ;
//    }

    @Override
    public Vec rayIntersect(Inters inters, Vec p, Vec d) {
        return null;
    }

    @Override
    public double minX() {
        return minx;
    }

    @Override
    public double minY() {
        return miny;
    }

    @Override
    public double minZ() {
        return minz;
    }

    @Override
    public double maxX() {
        return maxx;
    }

    @Override
    public double maxY() {
        return maxy;
    }

    @Override
    public double maxZ() {
        return maxz;
    }
}
