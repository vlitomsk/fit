package ru.nsu.ccfit.litvinov.cg.filter;

import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;

/**
 * Created by v on 3/21/16.
 */

@FunctionalInterface
interface Consumer5<A,B,C,D,E> {
    int accept(A a, B b, C c, D d, E e);
}

@FunctionalInterface
interface Consumer3<A,B,C> {
    int accept(A a, B b, C c);
}

class GraphicAlgo {
    protected static int satur(double x) {
        if (x < 0)
            return 0;
        if (x >= 255)
            return 255;
        return (int)Math.floor(x);
    }

    protected static int genRGB(int[] rgb) {
        return genRGB(rgb[0], rgb[1], rgb[2]);
    }

    protected static int genRGB(int r, int g, int b) {
        return 0xff000000 | (r << 16) | (g << 8) | b;
    }

    protected static int getR(int rgb) {
        return (rgb >> 16) & 0xff;
    }

    protected static int getG(int rgb) {
        return (rgb >> 8) & 0xff;
    }

    protected static int getB(int rgb) {
        return rgb & 0xff;
    }

    protected static int[] splRGB(int rgb) {
        int[] a = {getR(rgb), getG(rgb), getB(rgb)};
        return a;
    }
}

class VarSizeGraphicAlgo extends GraphicAlgo {
    public static BufferedImage rotate(BufferedImage bimg, double angle) {
        double sin = Math.sin(angle);
        double cos = Math.cos(angle);
        int w = bimg.getWidth();
        int h = bimg.getHeight();
        int w1 = (int)Math.round(h * Math.abs(sin) + w * Math.abs(cos));
        int h1 = (int)Math.round(h * Math.abs(cos) + w * Math.abs(sin));
        BufferedImage res = new BufferedImage(w1, h1, BufferedImage.TYPE_INT_RGB);
        for (int j = 0; j < h1; ++j) {
            for (int i = 0; i < w1; ++i) {
                int off_i = i - (int)Math.round(h*sin);
                int off_j = j;
                int src_i = (int)Math.round(off_i * cos + off_j * sin);
                int src_j = (int)Math.round(-off_i * sin + off_j * cos);
                int res_color = 0xffffff;
                if (src_i >= 0 && src_i < w && src_j >= 0 && src_j < h) {
                    res_color = bimg.getRGB(src_i, src_j);
                }
                res.setRGB(i, j, res_color);
            }
        }
        return res;
    }

    public static BufferedImage scale(BufferedImage bimg, double coef) {
        int w = bimg.getWidth();
        int w1 = (int)Math.round(w * coef);
        int h = bimg.getHeight();
        int h1 = (int)Math.round(h * coef);
        BufferedImage res = new BufferedImage(w1, h1, BufferedImage.TYPE_INT_RGB);
        for (int j = 0; j < h1; ++j) {
            for (int i = 0; i < w1; ++i) {
                int src_i = (int)Math.round(i / coef);
                int src_j = (int)Math.round(j / coef);
                res.setRGB(i, j, bimg.getRGB(src_i >= w ? w-1 : src_i, src_j >= h?h-1:src_j));
            }
        }
        return res;
    }
}

public class ConstSizeGraphicAlgo extends GraphicAlgo {
    static double[][] dithermap = {{1,9,3,11},{13,5,15,7},{4,12,2,10},{16,8,14,6}};
    static {
        for (int i = 0; i < dithermap.length; ++i) {
            for (int j = 0; j < dithermap[i].length; ++j) {
                dithermap[i][j] /= 17.0;
            }
        }
    }

    //static final int dithermask = 0b11100000;
    public static void orderDither(BufferedImage bimg, int dithermask) {
        pixelByPixel(bimg, (i,j,r,g,b) -> {
            double w = dithermap[i % 4][j % 4];
            int r1 = (int)Math.round(r + r * w);
            int g1 = (int)Math.round(g + g * w);
            int b1 = (int)Math.round(b + b * w);
            return genRGB(satur(r1)&dithermask,satur(g1)&dithermask,satur(b1)&dithermask);
        });
    }

    public static void steinbergDither(BufferedImage bimg, int nr, int ng, int nb) {
        int w = bimg.getWidth();
        int h = bimg.getHeight();
        int[] ns = {nr,ng,nb};
        int[] steps = {0,0,0};
        int[][] thresh = new int[3][Math.max(nr,Math.max(ng,nb))+1];
        for (int i = 0; i < 3; ++i) {
            steps[i] = 255 / ns[i];
            int j;
            for (j = 0; j < ns[i] ; ++j)
                thresh[i][j] = j * steps[i];
            thresh[i][j] = 255;
        }
        int[][] neigh = new int[5][3];
        double[] errweights = {0, 7.0/16.0, 1.0/16.0, 5.0/16.0, 3.0/16.0};
        int[] dxs = {0, 1, 1, 0, -1};
        int[] dys = {0, 0, 1, 1, 1};

        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w; ++i) {
                int[] rgb = splRGB(bimg.getRGB(i,j));
                for (int cidx = 0; cidx < 3; ++cidx) {
                    int thidx = rgb[cidx] / steps[cidx];
                    int err = rgb[cidx] - thresh[cidx][thidx];
                    neigh[0][cidx] = thresh[cidx][Math.min(thidx+1, thresh[cidx].length-1)] - rgb[cidx];
                    for (int nidx = 1; nidx < 5; ++nidx)
                        neigh[nidx][cidx] = (int)Math.round(err * errweights[nidx]);
                }

                for (int nidx = 0; nidx < 5; ++nidx) {
                    int dx = dxs[nidx];
                    int dy = dys[nidx];
                    if (i+dx < 0 || i+dx >= w || j+dy<0 || j+dy >= h)
                        continue;
                    int[] neigh_rgb = splRGB(bimg.getRGB(i+dx,j+dy));
                    for (int cidx = 0; cidx < 3; ++cidx)
                        neigh_rgb[cidx] = satur(neigh_rgb[cidx] + neigh[nidx][cidx]);
                    bimg.setRGB(i+dx,j+dy, genRGB(neigh_rgb));
                }
            }
        }
    }

    public static void desaturate(BufferedImage bimg) {
        final double rw = 0.2126;
        final double gw = 0.7152;
        final double bw = 1 - (rw - gw);
        pixelByPixel(bimg, (r, g, b) -> {
            int y = (int) Math.floor(rw * r + gw * g + bw * b);
            if (y > 0xff)
                y = 0xff;
            return genRGB(y, y, y);
        });
    }

    public static void gammaCorr(BufferedImage bimg, double gamma) {
        pixelByPixel(bimg, (r, g, b) -> {
            return genRGB(satur(Math.pow(r, gamma)), satur(Math.pow(g, gamma)), satur(Math.pow(b, gamma)));
        });
    }

    public static void negate(BufferedImage bimg) {
        pixelByPixel(bimg, (r, g, b) -> {
            return genRGB(0xff - r, 0xff - g, 0xff - b);
        });
    }

    public static void morethan(BufferedImage bimg, int rv, int gv, int bv, boolean binarize) {
        pixelByPixel(bimg, (r,g,b) -> {
            return (r > rv && g > gv && b > bv ? (binarize ? 0xffffff : genRGB(r,g,b)) : genRGB(0,0,0));
        });
    }

    private static void pixelByPixel(BufferedImage bimg, Consumer3<Integer,Integer,Integer> cons) {
        final int w = bimg.getWidth();
        final int h = bimg.getHeight();
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                final int rgb = bimg.getRGB(i, j);
                bimg.setRGB(i, j, cons.accept(getR(rgb), getG(rgb), getB(rgb)));
            }
        }
    }

    private static void pixelByPixel(BufferedImage bimg, Consumer5<Integer,Integer,Integer,Integer,Integer> cons) {
        final int w = bimg.getWidth();
        final int h = bimg.getHeight();
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                final int rgb = bimg.getRGB(i, j);
                bimg.setRGB(i, j, cons.accept(i,j,getR(rgb), getG(rgb), getB(rgb)));
            }
        }
    }

    public static BufferedImage deepCopy(BufferedImage bi) {
        ColorModel cm = bi.getColorModel();
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        WritableRaster raster = bi.copyData(null);
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
    }

    public static void convolution(BufferedImage bimg1, double[][] weight, boolean doNorm, boolean doAbs) {
        convolution(bimg1,weight, doNorm, doAbs, deepCopy(bimg1));
    }

    static double convsums[] = new double[3];
    public static void convolution(BufferedImage bimg1, double[][] weight, boolean doNorm, boolean doAbs, BufferedImage bimg) {
        if (doNorm) {
            double sum = 0;
            for (int i = 0; i < weight.length; ++i) {
                for (int j = 0; j < weight[0].length; ++j) {
                    sum += weight[i][j];
                }
            }
            for (int i = 0; i < weight.length; ++i) {
                for (int j = 0; j < weight[0].length; ++j) {
                    weight[i][j] /= sum;
                }
            }
        }
        final int w = bimg.getWidth();
        final int h = bimg.getHeight();

        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {

                for (int zz = 0; zz < 3; ++zz) {
                    convsums[zz] = 0;
                    for (int x = 0; x < weight.length; ++x) {
                        for (int y = 0; y < weight[x].length; ++y) {
                            int rx = i + x - weight.length / 2;
                            int ry = j + y - weight[x].length / 2;
                            if (rx < 0)
                                rx = 0;
                            if (ry < 0)
                                ry = 0;
                            if (rx >= w)
                                rx = w - 1;
                            if (ry >= h)
                                ry = h - 1;

                            int[] spl = splRGB(bimg.getRGB(rx, ry));
                            convsums[zz] += weight[x][y] * spl[zz];
                        }
                    }
                    if (convsums[zz] < 0) {
                        convsums[zz] = doAbs ? -convsums[zz] : 0;
                    }
                    if (convsums[zz] > 255)
                        convsums[zz] = 255;
//                    System.out.println("sums: " + convsums[zz]);

                }

                bimg1.setRGB(i, j, genRGB((int)Math.floor(convsums[0]),
                        (int)Math.floor(convsums[1]), (int)Math.floor(convsums[2])));
            }
        }
    }

    static final double[][] filtY = {{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}};
    public static void sobelY(BufferedImage bimg) {
        convolution(bimg, filtY, false,true);
    }

    static final double[][] filtX = {{-1,-2,-1}, {0,0,0}, {1,2,1}};
    public static void sobelX(BufferedImage bimg) {
        convolution(bimg, filtX, false, true);
    }

    static int []magarr = new int[3];
    public static void magnitude(BufferedImage bimg) {
        BufferedImage cp1 = deepCopy(bimg);
        BufferedImage cp2 = deepCopy(bimg);
        convolution(cp1, filtX, false, true, bimg);
        convolution(cp2, filtY, false, true, bimg);
        final int w = bimg.getWidth();
        final int h = bimg.getHeight();
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                int[] spl1 = splRGB(cp1.getRGB(i, j));
                int[] spl2 = splRGB(cp2.getRGB(i, j));
                for (int ch = 0; ch < 3; ++ch) {
                    magarr[ch] = satur((int)Math.floor(Math.sqrt(spl1[ch] * spl1[ch] + spl2[ch] * spl2[ch]) / Math.sqrt(2)));
                }
                bimg.setRGB(i, j, genRGB(magarr[0], magarr[1], magarr[2]));
            }
        }
    }

    static double[][] blurmat = null;
    public static void blur(BufferedImage bimg) {
        if (blurmat == null) {
            blurmat = new double[5][5];
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    blurmat[i][j] = 1;
                }
            }

        }
        convolution(bimg, blurmat, true, false);
    }

    static double[][] sharpenFilt = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};
    public static void sharpen(BufferedImage bimg) {
        convolution(bimg, sharpenFilt, false, false);
    }

    static double[][] aquaFilt1 = {{1,2,1}, {2,4,2}, {1,2,1}};
    static double[][] aquaFilt2 = {{-0.5, -0.5, -0.5}, {-0.5, 5, -0.5}, {-0.5, -0.5, -0.5}};
    public static void aqua(BufferedImage bimg) {
        convolution(bimg, aquaFilt1, true, false);
        convolution(bimg, aquaFilt2, false, false);
    }
}
