package ru.nsu.ccfit.litvinov.cg.life.model;

/**
 * Created by vas on 12.02.16.
 */
public class Rules {
    private static final double[] default_dstImp = {0, 1.0, 0.3};
    private static Vec[] default_neigh = {new Vec(-1, 0), new Vec(1, 0), new Vec(-1, -1), new Vec(1, -1), new Vec(-1, 1), new Vec(1, 1),
     new Vec(-2, -1), new Vec(2, -1), new Vec(-2, 1), new Vec(2, 1), new Vec(0, 2), new Vec(0, -2)};
    public static final Rules DEFAULT = new Rules(default_dstImp, default_neigh, 2.0, 3.3, 2.3, 2.9);

    double[] distImpacts;
    int[] dists;
    Vec[] neighborsRel;
    double liveBegin, liveEnd, birthBegin, birthEnd;

    public Rules(double[] distImpact, Vec[] neighborsRel, double liveBegin, double liveEnd, double birthBegin, double birthEnd) {
        if (liveBegin < 0 || liveEnd < 0 || birthBegin < 0 || birthEnd < 0 ||
                !(liveBegin <= birthBegin && birthBegin <= birthEnd && birthEnd <= liveEnd))
            throw new IllegalArgumentException("Given args don't satisfy: live_begin <= birth_begin <= birthEnd <= liveEnd (or negative)");
        this.distImpacts = distImpact;
        this.liveBegin = liveBegin;
        this.liveEnd = liveEnd;
        this.birthBegin = birthBegin;
        this.birthEnd = birthEnd;
        this.neighborsRel = neighborsRel;
        int maxDist = distImpact.length - 1;
        dists = new int[neighborsRel.length];
        for (int i = 0; i < dists.length; ++i) {
            dists[i] = Vec.zero.dist(neighborsRel[i]);
            if (dists[i] > maxDist)
                throw new IllegalArgumentException("There's no impact for distance: " + dists[i] + " // " + neighborsRel[i]);
        }
    }

    private boolean bounds(double a, double b, double x) {
        return a <= x && x <= b;
    }

    boolean birthCond(double impact) {
        return bounds(birthBegin, birthEnd, impact);
    }

    boolean liveCond(double impact) {
        return bounds(liveBegin, liveEnd, impact);
    }

    boolean dieCond(double impact) {
        return impact < liveBegin || impact > liveEnd;
    }
}
