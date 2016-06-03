package ru.vlitomsk.raytr.geom.render;

import ru.vlitomsk.raytr.geom.base.Vec;

import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import static ru.vlitomsk.raytr.geom.base.IOUtil.*;
/**
 * Created by vas on 31.05.2016.
 */
public class CameraReader {
    public static ViewportCameraSettings readSettings(InputStream is) throws IOException {
        ViewportCameraSettings sett = new ViewportCameraSettings();
        BufferedReader br = new BufferedReader(new InputStreamReader(is));
        String[] sp = getl(br);
        Color bgcolor = readCol(sp, 0);
        double gamma = Double.parseDouble(getl(br)[0]);
        int raytrDepth = Integer.parseInt(getl(br)[0]);
        String qualStr = getl(br)[0].trim();
        int qual;
        if (qualStr.equals("ROUGH")) {
            qual = 0;
        } else if (qualStr.equals("NORMAL")) {
            qual = 1;
        } else if (qualStr.equals("FINE")) {
            qual = 2;
        } else {
            throw new RuntimeException("Unknown quality type! " + qualStr);
        }
        sp = getl(br);
        Vec camPos = readVec3(sp, 0);
        sp = getl(br);
        Vec camView = readVec3(sp, 0);
        sp = getl(br);
        Vec camUp = readVec3(sp, 0);
        sp = getl(br);
        double zf = Double.parseDouble(sp[0]);
        double zb = Double.parseDouble(sp[1]);
        sp = getl(br);
        double Sw = Double.parseDouble(sp[0]);
        double Sh = Double.parseDouble(sp[1]);

        sett.setCamPos(camPos);
        sett.setCamView(camView);
        sett.setCamUp(camUp);
        sett.setBgColor(bgcolor);
        sett.setZb(-zb);
        sett.setZf(-zf);
        sett.setSw(Sw);
        sett.setSh(Sh);
        return sett;
    }
}
