package ru.vlitomsk.raytr.geom.base;

import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;

/**
 * Created by vas on 31.05.2016.
 */
public class IOUtil {
    public static String[] getl(BufferedReader br) throws IOException {
        String s = br.readLine();
        if (s == null)
            return null;
        String[] tmp = s.split("//");
        s = tmp[0].trim();
        if (s.length() == 0)
            return getl(br);
        return s.split(" ");
    }

    public static Color readCol(String[] sp, int off) {
        int r = Integer.parseInt(sp[off+0]);
        int g = Integer.parseInt(sp[off+1]);
        int b = Integer.parseInt(sp[off+2]);
        return new Color(r,g,b);
    }

    public static Vec readVec3(String[] sp, int off) {
        Vec v = new Vec();
        for (int i = 0; i < 3; ++i)
            v.comp[i] = Double.parseDouble(sp[off+i]);
        v.comp[3] = 1;
        return v;
    }

}
