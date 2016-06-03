package ru.nsu.ccfit.litvinov.cg.life.model;

/**
 * Created by vas on 12.02.16.
 */
class Vec {
    static final Vec zero = new Vec(0, 0);
    int x, y;

    public Vec() {
        x = y = 0;
    }

    public Vec(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int dist(Vec v) {
        return weight(v.x - x, v.y - y);
    }

    public static int weight(int x, int y) {
        return Math.max(Math.abs(x), Math.abs(y));
    }

    @Override
    public String toString() {
        return "vec " + x + " " + y;
    }

    public void addPlace(Vec v) {
        this.x += v.x;
        this.y += v.y;
    }

    public void set(Vec v) {
        this.x = v.x;
        this.y = v.y;
    }
}
