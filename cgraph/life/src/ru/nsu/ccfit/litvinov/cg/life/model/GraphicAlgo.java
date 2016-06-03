package ru.nsu.ccfit.litvinov.cg.life.model;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.util.List;
import java.util.ArrayList;

public class GraphicAlgo {
    public static void bresenhamLine(Color color, BufferedImage img, int x0, int y0, int x1, int y1) {
        if (x0 > x1)
            bresenhamLineOrd(color, img, x1, y1, x0, y0);
        else
            bresenhamLineOrd(color, img, x0, y0, x1, y1);
    }

    private static void bresenhamLineOrd(Color color, BufferedImage img, int x0, int y0, int x1, int y1) {
        final int rgbcolor = color.getRGB();
        if (x0 == x1) {
            int yto = Math.max(y0, y1);
            for (int i = Math.min(y0, y1); i <= yto; ++i) {
                img.setRGB(x0, i, rgbcolor);
            }
            return;
        }
        int deltax = Math.abs(x1 - x0);
        final int deltay = Math.abs(y1 - y0);
        int error = 0;
        int deltaerr = deltay;
        final int yinc = y0 > y1 ? -1 : 1;
        int y = y0;
        for (int x = x0; x <= x1 ; ++x) {
            img.setRGB(x, y, rgbcolor);
            error += deltaerr;
            if (2 * error >= deltax) {
                y += yinc;
                error -= deltax;
            }
        }
    }

    public static void spanFill(Color color, BufferedImage img, int x0, int y0) {
        final int fillrgb = color.getRGB();
        final int bgrgb = img.getRGB(x0, y0);
        final int W = img.getWidth();
        final int H = img.getHeight();
        List<Point> seeds = new ArrayList<>();

        for (; x0 >= 0 && img.getRGB(x0, y0) == bgrgb; --x0) {
        }
        ++x0;
        seeds.add(new Point(x0, y0));
        final byte ST_NONBG = 0, ST_BG = 1;
        while (!seeds.isEmpty()) {
            Point seed = seeds.remove(0);
            byte states[] = {ST_NONBG, ST_NONBG};
            for (int x = seed.x; x < img.getWidth() && img.getRGB(x, seed.y) == bgrgb; ++x) {
                img.setRGB(x, seed.y, fillrgb);
                for (int dy = -1; dy < 2; dy += 2) {
                    final int new_y = seed.y + dy;
                    if (new_y < 0 || new_y >= img.getWidth())
                        continue;
                    final int stidx = dy == -1 ? 0 : 1;
                    final int pix = img.getRGB(x, new_y);
                    switch (states[stidx]) {
                        case ST_BG:
                            if (pix != bgrgb)
                                states[stidx] = ST_NONBG;
                            break;
                        case ST_NONBG:
                            if (pix == bgrgb) {
                                states[stidx] = ST_BG;
                                seeds.add(new Point(x, new_y));
                            }
                            break;
                    }
                }
            }
        }
    }
}
