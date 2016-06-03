package ru.vlitomsk.raytr.geom.render.intersect;

import ru.vlitomsk.raytr.geom.base.Vec;

import java.util.*;
import java.util.function.*;
import static java.lang.Math.*;

/**
 * Created by vas on 30.05.2016.
 */
public class Octree<Tp extends BoundedObject> implements BoundedObject {
    private Octree subtree[][][] = new Octree[2][2][2];
    private List<Tp> container = new ArrayList<>();
    private static final int MIN=0, MID=1, MAX=2;
    private double[] xc = new double[3];
    private double[] yc = new double[3];
    private double[] zc = new double[3];
    private double[][] coord = {xc, yc, zc};

    public Octree(double x1, double x2, double y1, double y2, double z1, double z2) {
        xc[MIN] = min(x1,x2);
        xc[MAX] = max(x1,x2);
        yc[MIN] = min(y1,y2);
        yc[MAX] = max(y1,y2);
        zc[MIN] = min(z1,z2);
        zc[MAX] = max(z1,z2);
        for (int i = 0; i < 3; ++i) {
            coord[i][MID] = (coord[i][MIN] + coord[i][MAX]) * 0.5;
        }
    }

    public void allRayIntersections(Vec raypt, Vec rayNormalizedDir, Consumer<Tp> cons) {
        if (!intersectsWithRay(raypt, rayNormalizedDir))
            return;

        container.forEach(cons);
        forAllSubtree((i,j,k) -> {
            if (subtree[i][j][k] != null)
                subtree[i][j][k].allRayIntersections(raypt, rayNormalizedDir, cons);
        });
    }

    public void allRayIntersectionsBreakable(Vec raypt, Vec rayNormalizedDir, ToIntFunction<Tp> cons) {
        if (!intersectsWithRay(raypt, rayNormalizedDir))
            return;
        for (Tp item : container) {
            int res = cons.applyAsInt(item);
            if (res == -1)
                return;
        }
        forAllSubtree((i,j,k) -> {
            if (subtree[i][j][k] != null)
                subtree[i][j][k].allRayIntersectionsBreakable(raypt, rayNormalizedDir, cons);
        });
    }

    private boolean intersectsWithRay(Vec raypt, Vec rayNormDir) {
      //  return true;
        if (bound(xc[MIN], xc[MAX], raypt.x()) &&
                bound(yc[MIN], yc[MAX], raypt.y()) &&
                bound(zc[MIN], zc[MAX], raypt.z()))
            return true;
        double tnear = Double.MIN_VALUE, tfar = Double.MAX_VALUE;
        double xl, xh, x0, xd, t1, t2, tmp;

        for (int cidx = 0; cidx < 3; ++cidx) {
            if (raypt.comp[cidx] != 0) {
                xl = coord[cidx][MIN]; xh = coord[cidx][MAX];
                x0 = raypt.comp[cidx]; xd = rayNormDir.comp[cidx];
//                if (x0 < xl || x0 > xh)
//                    return false;
                t1 = (xl - x0) / xd;
                t2 = (xh - x0) / xd;
                if (t1 > t2) {
                    tmp = t2;
                    t2 = t1;
                    t1 = tmp;
                }

                tnear = max(tnear, t1);
                tfar = min(tfar, t2);
                if (tnear > tfar || tfar < 0)
                    return false;
            }
        }

        return true;
    }

    public void add(Tp obj, int maxDepth) {
        if (!in(0, 0, 0, 2, obj))
            throw new RuntimeException("Object is out of octree");

        if (maxDepth != 0) {
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                    for (int k = 0; k < 2; ++k)
                        if (in(i, j, k, 1, obj)) {
                            if (subtree[i][j][k] == null)
                                subtree[i][j][k] = new Octree(xc[i], xc[i+1], yc[j], yc[j+1], zc[k], zc[k+1]);
                            subtree[i][j][k].add(obj, maxDepth - 1);
                            return;
                        }
        }
        //Gstate.counter++;
        container.add(obj);
//        System.out.println("gstate counter:" + Gstate.counter);
    }

    @FunctionalInterface
    interface IdxConsumer {
        public void accept(int i, int j, int k);
    }

    private void forAllSubtree(IdxConsumer cons) {
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                    cons.accept(i,j,k);
    }

    private static boolean bound(double a, double b, double v) {
        return a < v && v < b;
    }

    private boolean in(int i, int j, int k, int delta, BoundedObject inner) {
        return inner.minX() >= xc[i] && inner.maxX() <= xc[i+delta] &&
               inner.minY() >= yc[j] && inner.maxY() <= yc[j+delta] &&
               inner.minZ() >= zc[k] && inner.maxZ() <= zc[k+delta];
    }

    @Override
    public double minX() {
        return xc[MIN];
    }

    @Override
    public double minY() {
        return yc[MIN];
    }

    @Override
    public double minZ() {
        return zc[MIN];
    }

    @Override
    public double maxX() {
        return xc[MAX];
    }

    @Override
    public double maxY() {
        return yc[MAX];
    }

    @Override
    public double maxZ() {
        return zc[MAX];
    }
}
