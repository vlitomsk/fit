package ru.nsu.ccfit.litvinov.cg.isolines;

import java.awt.*;
import java.util.*;
import java.io.*;

/**
 * Created by v on 4/1/16.
 */
public class Settings {
    ViewSettings vs;
    ModelSettings ms;
    private static Color readColor(Scanner scan) throws IOException {
        int r, g, b;
        r = scan.nextInt();
        g = scan.nextInt();
        b = scan.nextInt();
        return new Color(r, g, b, 0xff);
    }

    public static Settings readSettings(InputStream is) throws IOException {
        Settings res = new Settings();
        Scanner scan = new Scanner(is);
        res.ms = new ModelSettings();
        res.ms.k = scan.nextInt();
        res.ms.m = scan.nextInt();
        res.ms.n = scan.nextInt();

        res.vs = new ViewSettings();
        res.vs.legendColors = new Color[res.ms.n + 1];
        for (int i = 0; i < res.ms.n + 1; ++i) {
            res.vs.legendColors[i] = readColor(scan);
        }
        res.vs.isoColor = readColor(scan);
        return res;
    }

    private static StringBuilder appendColor(StringBuilder sb, Color c) {
        return sb.append(c.getRed()).append(' ').append(c.getGreen()).append(' ').append(c.getBlue());
    }

    public static void writeSettings(ModelSettings ms, ViewSettings vs, OutputStream os) throws IOException {
        StringBuilder sb = new StringBuilder();
        sb.append(ms.k).append(' ').append(ms.m).append('\n');
        sb.append(ms.n).append('\n');
        for (int i = 0; i < ms.n+1; ++i) {
            appendColor(sb, vs.legendColors[i]).append('\n');
        }
        appendColor(sb, vs.isoColor).append('\n');
        os.write(sb.toString().getBytes());
    }
}
