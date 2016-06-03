package ru.nsu.ccfit.litvinov.cg.filter.bmpformat;

import java.awt.image.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by v on 3/21/16.
 */
public class BitmapImage {
    private BitmapImage() {}
    public static BufferedImage deepCopy(BufferedImage bi) {
        ColorModel cm = bi.getColorModel();
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        WritableRaster raster = bi.copyData(null);
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
    }
    public BitmapImage(BufferedImage img) {
        this.img = deepCopy(img);
        bfType = 0x4d42;
        bfOffBits = 54;
        bcSize = 40;
        version = 1;
        info3 = new Info3();
        info3.biWidth = img.getWidth();
        info3.biHeight = img.getHeight();
        info3.biPlanes = 1;
        info3.biBitCount = 24;
        info3.biSizeImage = info3.biWidth * info3.biHeight * 3;
    }
    private static int readW(InputStream is) throws IOException {
        int a = is.read();
        return (is.read() << 8) | a;
    }

    private static void writeW(OutputStream os, int w) throws IOException {
        os.write((byte)(w & 0xff));
        os.write((byte)((w >> 8) & 0xff));
    }

    private static void writeDW(OutputStream os, long dw) throws IOException {
        writeW(os, (int)(dw & 0xffff));
        writeW(os, (int)((dw >> 16) & 0xffff));
    }

    private static long readDW(InputStream is) throws IOException {
        long res = 0;
        for (int i = 0; i < 4; ++i) {
            res |= (((long)is.read()) << (i << 3));
        }
        return res;
    }

    int bfType;
    long bfSize;
    int bfReserved1, bfReserved2;
    long bfOffBits;
    long bcSize;
    static final int VER_CORE = 0, VER_3 = 1, VER_4 = 2, VER_5 = 3;
    int version;

    static final int HEADER_SZ_BYTES = 14;

    InfoCore infocore = null;
    static final int INFOCORE_SZ_BYTES = 12;
    Info3 info3 = null;
    static final int INFO3_SZ_BYTES = 40;
    Info4 info4 = null;
    static final int INFO4_SZ_BYTES = INFO3_SZ_BYTES + 68;
    Info5 info5 = null;
    static final int INFO5_SZ_BYTES = INFO4_SZ_BYTES + 16;

    BufferedImage img;

    public BufferedImage getImg() {
        return img;
    }

    //int[] pixels;

    private static InfoCore loadInfoCore(InputStream is) throws IOException, BadBMPFormatException {
        InfoCore ic = new InfoCore();
        ic.bcWidth = readW(is);
        ic.bcHeight = readW(is);
        ic.bcPlanes = readW(is);
        if (ic.bcPlanes != 1) {
            throw new BadBMPFormatException("bcPlanes != 1");
        }
        ic.bcBitCount = readW(is);
        if (ic.bcBitCount != 24) {
            throw new BadBMPFormatException("This bcBitCount is not supported: " + String.valueOf(ic.bcBitCount));
        }
        return ic;
    }

    private static Info3 loadInfo3(InputStream is) throws IOException, BadBMPFormatException {
        Info3 i3 = new Info3();
        i3.biWidth = readDW(is);
        i3.biHeight = readDW(is);
        i3.biPlanes = readW(is);
        if (i3.biPlanes != 1)
            throw new BadBMPFormatException("biPlanes != 1");
        i3.biBitCount = readW(is);
        if (i3.biBitCount != 24)
            throw new BadBMPFormatException("i3.biBitCount != 24");
        i3.biCompression = readDW(is);
        i3.biSizeImage = readDW(is);
        i3.biXPelsPerMeter = readDW(is);
        i3.biYPelsPerMeter = readDW(is);
        i3.biClrUsed = readDW(is);
        i3.biClrImportant = readDW(is);
        return i3;
    }

    private static Info4 loadInfo4(InputStream is) throws IOException, BadBMPFormatException {
        Info4 i4 = new Info4();
        i4.i3 = loadInfo3(is);
        i4.bV4RedMask = readDW(is);
        i4.bV4GreenMask = readDW(is);
        i4.bV4BlueMask = readDW(is);
        i4.bV4AlphaMask = readDW(is);
        i4.bV4CSType = readDW(is);
        i4.bV4Endpoints = new byte[Info4.bV4EndpointsSize];
        is.read(i4.bV4Endpoints);
        i4.bV4GammaRed = readDW(is);
        i4.bV4GammaGreen = readDW(is);
        i4.bV4GammaBlue = readDW(is);
        return i4;
    }

    private static Info5 loadInfo5(InputStream is) throws IOException, BadBMPFormatException {
        Info5 i5 = new Info5();
        i5.i4 = loadInfo4(is);
        i5.bV5Intent = readDW(is);
        i5.bV5ProfileData = readDW(is);
        i5.bV5ProfileSize = readDW(is);
        i5.bV5Reserved = readDW(is);
        if (i5.bV5Reserved != 0)
            throw new BadBMPFormatException("i5.bV5Reserved != 0");
        return i5;
    }

    public static BitmapImage load(InputStream is) throws IOException, BadBMPFormatException {
        BitmapImage b = new BitmapImage();

        b.bfType = readW(is);
        if (b.bfType != 0x4d42) {
            throw new BadBMPFormatException("bfType != 0x4d42");
        }

        b.bfSize = readDW(is);
        b.bfReserved1 = readW(is);
        b.bfReserved2 = readW(is);
        if (b.bfReserved1 != 0 || b.bfReserved2 != 0) {
            throw new BadBMPFormatException("bfReserved1 != 0 || bfReserved2 != 0");
        }
        b.bfOffBits = readDW(is);
        b.bcSize = readDW(is);

        int pos = HEADER_SZ_BYTES;
        if (b.bcSize == 12) {
            b.version = VER_CORE;
            b.infocore = loadInfoCore(is);
            pos += INFOCORE_SZ_BYTES;
        } else if (b.bcSize == 40) {
            b.version = VER_3;
            b.info3 = loadInfo3(is);
            pos += INFO3_SZ_BYTES;
        } else if (b.bcSize == 108) {
            b.version = VER_4;
            b.info4 = loadInfo4(is);
            b.info3 = b.info4.i3;
            pos += INFO4_SZ_BYTES;
        } else if (b.bcSize == 124) {
            b.version = VER_5;
            b.info5 = loadInfo5(is);
            b.info4 = b.info5.i4;
            b.info3 = b.info4.i3;
            pos += INFO5_SZ_BYTES;
        } else {
            throw new BadBMPFormatException("Bad bcSize value (can't identify BMP version)");
        }

        is.skip(b.bfOffBits - pos);
        int pixelsSize = (int) (3 * b.info3.biWidth * b.info3.biHeight);
        byte[] pixels = new byte[pixelsSize];
        int cnt = is.read(pixels);
        if (cnt != pixelsSize) {
            throw new BadBMPFormatException("Unexpected end of bitmap! (can't read pixel data)");
        }
        int[] pixels2 = new int[(int)(b.info3.biWidth * b.info3.biHeight)];

        int pidx = 0;
        for (int i = (int)b.info3.biWidth - 1; i >= 0; --i) {

            for (int j = (int)b.info3.biHeight - 1; j >= 0; --j) {
                final int bl = pixels[pidx * 3] & 0xff;
                final int gr = pixels[pidx * 3 + 1] & 0xff;
                final int re = pixels[pidx * 3 + 2] & 0xff;
                pixels2[ i * (int)b.info3.biHeight + j] = bl | (gr << 8) | (re << 16) | 0xff000000;
                ++pidx;
            }

        }

        int[] bitMasks = {0xff0000, 0xff00, 0xff, 0xff000000};
        DataBufferInt dbi = new DataBufferInt(pixels2, pixels2.length);

        WritableRaster wr = Raster.createPackedRaster(
                dbi,
                (int) b.info3.biWidth,
                (int) b.info3.biHeight,
                (int) b.info3.biWidth, bitMasks,
                new java.awt.Point(0,0));
        b.img = new BufferedImage(ColorModel.getRGBdefault(), wr, false, null);

        flip(b.img);

        return b;
    }

    private static void saveInfoCore(OutputStream os, InfoCore ic) throws IOException {
        writeW(os, ic.bcWidth);
        writeW(os, ic.bcHeight);
        writeW(os, ic.bcPlanes);
        writeW(os, ic.bcBitCount);
    }

    private static void saveInfo3(OutputStream os, Info3 i3) throws IOException {
        writeDW(os, i3.biWidth);
        writeDW(os, i3.biHeight);
        writeW(os, i3.biPlanes);
        writeW(os, i3.biBitCount);
        writeDW(os, i3.biCompression);
        writeDW(os, i3.biSizeImage);
        writeDW(os, i3.biXPelsPerMeter);
        writeDW(os, i3.biYPelsPerMeter);
        writeDW(os, i3.biClrUsed);
        writeDW(os, i3.biClrImportant);
    }

    private static void saveInfo4(OutputStream os, Info4 i4) throws IOException {
        saveInfo3(os, i4.i3);
        writeDW(os, i4.bV4RedMask);
        writeDW(os, i4.bV4GreenMask);
        writeDW(os, i4.bV4BlueMask);
        writeDW(os, i4.bV4AlphaMask);
        writeDW(os, i4.bV4CSType);
        os.write(i4.bV4Endpoints);
        writeDW(os, i4.bV4GammaRed);
        writeDW(os, i4.bV4GammaGreen);
        writeDW(os, i4.bV4GammaBlue);
    }

    private static void saveInfo5(OutputStream os, Info5 i5) throws IOException {
        saveInfo4(os, i5.i4);
        writeDW(os, i5.bV5Intent);
        writeDW(os, i5.bV5ProfileData);
        writeDW(os, i5.bV5ProfileSize);
        writeDW(os, i5.bV5Reserved);
    }

    public static void save(BitmapImage b, OutputStream os) throws IOException {
        writeW(os, b.bfType);
        writeDW(os, b.bfSize);
        writeW(os, b.bfReserved1);
        writeW(os, b.bfReserved2);
        writeDW(os, b.bfOffBits);
        writeDW(os, b.bcSize);
        int pos = HEADER_SZ_BYTES;
        if (b.version == VER_CORE) {
            saveInfoCore(os, b.infocore);
            pos += INFOCORE_SZ_BYTES;
        } else if (b.version == VER_3) {
            saveInfo3(os, b.info3);
            pos += INFO3_SZ_BYTES;
        } else if (b.version == VER_4) {
            saveInfo4(os, b.info4);
            pos += INFO4_SZ_BYTES;
        } else if (b.version == VER_5) {
            saveInfo5(os, b.info5);
            pos += INFO5_SZ_BYTES;
        }

        // палитра
        for (int toSkip = (int)b.bfOffBits - pos; toSkip > 0; --toSkip) {
            os.write(0);
        }

        final int w = b.img.getWidth();
        final int h = b.img.getHeight();
        byte[] pixels = new byte[w * h * 3];
        int pcount = 0;

        for (int j = h - 1; j >= 0; --j) {
            for (int i = 0; i < w; ++i) {
                int rgb = b.img.getRGB(i, j);
                pixels[pcount * 3] = (byte)(rgb & 0xff);
                pixels[pcount * 3 + 1] = (byte) ((rgb >> 8) & 0xff);
                pixels[pcount * 3 + 2] = (byte) ((rgb >> 16) & 0xff);
                ++pcount;
            }
        }

        os.write(pixels);
    }

    public static void flip(BufferedImage image) {
        final int h = image.getHeight();
        final int w = image.getWidth();
        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w / 2; ++i) {
                int tmp = image.getRGB(i, j);
                image.setRGB(i, j, image.getRGB(w - i - 1, j));
                image.setRGB(w - i - 1, j, tmp);
            }
        }
    }
}
