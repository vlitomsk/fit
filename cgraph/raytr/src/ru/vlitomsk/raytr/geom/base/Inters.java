package ru.vlitomsk.raytr.geom.base;

/**
 * Created by vas on 17.05.2016.
 */

import ru.vlitomsk.raytr.geom.base.Vec;

/**
 * надо создавать один Inters на один поток
 */
public class Inters {
    Vec n = new Vec(0,0);
    Vec sub1 = new Vec();
    Vec sub2 = new Vec();
    Vec Q = new Vec();

    public Vec rayTr(Vec p, Vec d, Vec A, Vec B, Vec C) {
        //d.pnormalize3d();
        n.assign3(B).padd(A, -1.0).pcross3d(sub1.assign3(C).padd(A, -1.0)).pnormalize3d();
        final double nd = n.dot(3, d);
        if (Math.abs(nd) < 1e-9) {
            return null; // no isection
        }
        double planeD = n.dot(3, A);
        double t = (planeD - n.dot(3, p)) / nd;
        if (t < 0) // Это луч а не прямая
            return null;
        //Vec Q = p.add(d, t);
        Q.assign3(p).padd(d, t);
        Q.comp[3] = 1;
        boolean fl1 = sub1.assign3(B).padd(A, -1).pcross3d(sub2.assign3(Q).padd(A, -1)).dot(3, n) >= 0;
        boolean fl2 = sub1.assign3(C).padd(B, -1).pcross3d(sub2.assign3(Q).padd(B, -1)).dot(3, n) >= 0;
        boolean fl3 = sub1.assign3(A).padd(C, -1).pcross3d(sub2.assign3(Q).padd(C, -1)).dot(3, n) >= 0;
        boolean intersection = (fl1 && fl2 && fl3) || (!fl1 && !fl2 && !fl3);
        return intersection ? Q : null;
    }

    public Vec rayQuad(Vec p, Vec d, Vec p0, Vec p1, Vec p2, Vec p3) {
        Vec q = rayTr(p, d, p0, p1, p2);
        return q == null ? rayTr(p, d, p0, p2, p3) : q;
    }
}
