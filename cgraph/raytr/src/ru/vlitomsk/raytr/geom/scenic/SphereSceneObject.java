package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Inters;
import ru.vlitomsk.raytr.geom.base.Segment;
import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.transform.Transform;

import java.util.ArrayList;

/**
 * Created by vas on 17.05.2016.
 */
public class SphereSceneObject extends SceneObject {
    private Vec center;
    private Vec pt;
    private double radius, sqradius;
    public SphereSceneObject(Vec center, double radius) {
        this.center = center.clone();
        this.radius = radius;
        sqradius=radius*radius;
        pt = center.clone();
        pt.comp[0] += radius;
    }

    private SphereSceneObject(){center = new Vec(); pt=new Vec();}

    @Override
    public SceneObject clone() {
        SphereSceneObject s = new SphereSceneObject();
        s.assign(this);
        s.setOpticParams(opticParams);
        return s;
    }

    @Override
    public void assign(SceneObject obj) {
        SphereSceneObject sp = (SphereSceneObject) obj;
        pt.assign3(sp.pt);
        center.assign3(sp.center);
        radius = sp.radius;
        sqradius=radius*radius;
    }

    @Override
    public void ptransform(Transform tr) {
        tr.ptransform(pt);
        tr.ptransform(center);
        radius = OC.assign3(pt).padd(center, -1).norm3d();
        sqradius=radius*radius;
    }

    @Override
    public boolean atEdge(Vec v) {
        return false;
    }

    Vec OC = new Vec();
    Vec tmp = new Vec();
    Vec tmp2 = new Vec();
    @Override
    public Vec rayIntersect(Inters inters, Vec p, Vec d) {
        OC.assign3(center).padd(p, -1);
        boolean insphere = OC.sqrNorm3d() < sqradius;
        double tsv = OC.dot(3,d);
        if (tsv<0 && !insphere)
            return null;
        double thc2 = sqradius - OC.sqrNorm3d() + tsv*tsv;
        if (thc2 < 0)
            return null;
        double thc = Math.sqrt(thc2);
        double t = insphere ? tsv + thc : tsv - thc;
        return tmp2.assign3(p).padd(d, t);
    }

    @Override
    public Vec calcNorm(Vec pt, Vec v) {
        return v.assign3(pt).padd(center, -1).pcoef3(1/radius);
    }

    private static final int N = 10; // колво обручей
    private static final int M = 10; // разбиение на сектора
    private ArrayList<Segment> allSegs = new ArrayList<>();

    private void addCircle(Vec center, double rad) {
        double dphi = Math.PI * 2.0 / M;
        Vec v1, v2 = null;
        for (int i = 0; i < M; ++i) {
            v1 = new Vec(rad * Math.cos(dphi * i), rad * Math.sin(dphi * i)).padd(center);
            v2 = new Vec(rad * Math.cos(dphi * (i + 1)), rad * Math.sin(dphi * (i + 1))).padd(center);
            allSegs.add(new Segment(v1, v2));
        }
        v2 = v2.clone();
        v1 = new Vec(rad, 0).padd(center);
        allSegs.add(new Segment(v2, v1));
    }

    private void addSegments() {
        double diam = 2 * radius;
        double ddiam = diam / N;
        for (int i = 0; i < N - 1; ++i) {
            double dz = (ddiam * (i+1) - radius);
            double rad = Math.sqrt(sqradius -dz*dz);
            addCircle(center.clone().padd(new Vec(0, 0, dz)), rad);
            //addCircle(center.clone().padd(new Vec(dz, 0, 0)), rad);
        }
    }

    @Override
    public Segment[] getSegments() {
        allSegs.clear();
        addSegments();
        Segment[] segs = new Segment[allSegs.size()];
        for (int i = 0; i < segs.length; ++i) {
            segs[i] = allSegs.get(i);
        }
        return segs;
        //return (Segment[])allSegs.toArray();
    }

    @Override
    public double minX() {
        return center.x()-radius;
    }

    @Override
    public double minY() {
        return center.y()-radius;
    }

    @Override
    public double minZ() {
        return center.z()-radius;
    }

    @Override
    public double maxX() {
        return center.x()+radius;
    }

    @Override
    public double maxY() {
        return center.y()+radius;
    }

    @Override
    public double maxZ() {
        return center.z()+radius;
    }
}
