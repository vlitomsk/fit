package ru.vlitomsk.raytr.geom.scenic;

import ru.vlitomsk.raytr.geom.base.Vec;

import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import static ru.vlitomsk.raytr.geom.base.IOUtil.*;
/**
 * Created by vas on 30.05.2016.
 */
public class SceneReader {
    public static Scene readScene(InputStream is) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(is));
        String[] sp = getl(br);
        Color diffuseColor = readCol(sp, 0);
        int nlight = Integer.parseInt(getl(br)[0]);
        Light[] lights = new Light[nlight];
        for (int i = 0; i < nlight; ++i) {
            sp = getl(br);
            Vec lightpos = readVec3(sp, 0);
            Color lightcolor = readCol(sp, 3);
            lights[i] = new Light(lightpos, lightcolor);
        }
        ArrayList<SceneObject> sobj = new ArrayList<SceneObject>();
        while (true) {
            sp = getl(br);
            if (sp == null)
                break;
            SceneObject obj;
            switch (sp[0].charAt(0)) {
                case 'S':
                    sobj.add(obj=readSphere(br));
                    break;
                case 'B':
                    sobj.add(obj=readBox(br));
                    break;
                case 'T':
                    sobj.add(obj=readTriangle(br));
                    break;
                case 'Q':
                    sobj.add(obj=readQuadrangle(br));
                    break;
                default:
                    throw new RuntimeException("unknown scene object type! " + sp[0]);
            }
            OpticParams opar = readOpticParams(br);
            obj.setOpticParams(opar);
        }

        return new Scene() {
            @Override
            public Color getDiffuseLightColor() {
                return diffuseColor;
            }

            @Override
            public SceneObject[] getSceneObjects() {
                SceneObject[] arr = new SceneObject[sobj.size()];
                sobj.toArray(arr);
                return arr;
            }

            @Override
            public Light[] getLights() {
                return lights;
            }
        };
    }

    private static SceneObject readQuadrangle(BufferedReader br) throws IOException {
        String[] sp = getl(br);
        Vec p0 = readVec3(sp, 0); sp = getl(br);
        Vec p1 = readVec3(sp, 0); sp = getl(br);
        Vec p2 = readVec3(sp, 0); sp = getl(br);
        Vec p3 = readVec3(sp, 0);
        return new QuadrangleSceneObject(p0, p1, p2, p3);
    }

    private static SceneObject readTriangle(BufferedReader br) throws IOException {
        String[] sp = getl(br);
        Vec p0 = readVec3(sp, 0); sp = getl(br);
        Vec p1 = readVec3(sp, 0); sp = getl(br);
        Vec p2 = readVec3(sp, 0);
        return new TriangleSceneObject(p0, p1, p2);
    }

    private static SceneObject readBox(BufferedReader br) throws IOException {
        String[] sp = getl(br);
        double minx,miny,minz,maxx,maxy,maxz;
        minx = Double.parseDouble(sp[0]);
        miny = Double.parseDouble(sp[1]);
        minz = Double.parseDouble(sp[2]);
        sp = getl(br);
        maxx = Double.parseDouble(sp[0]);
        maxy = Double.parseDouble(sp[1]);
        maxz = Double.parseDouble(sp[2]);
        return new BoxSceneObject(minx,maxx,miny,maxy,minz,maxz);
    }

    private static SceneObject readSphere(BufferedReader br) throws IOException {
        String[] sp = getl(br);
        Vec center = readVec3(sp, 0); sp = getl(br);
        double radius = Double.parseDouble(sp[0]);
        return new SphereSceneObject(center, radius);
    }

    private static OpticParams readOpticParams(BufferedReader br) throws  IOException {
        String[] sp = getl(br);
        Vec KD = readVec3(sp, 0); Vec KS = readVec3(sp, 3);
        double power = Double.parseDouble(sp[6]);
        return new OpticParams(null, KD, KS, power);
    }
}
