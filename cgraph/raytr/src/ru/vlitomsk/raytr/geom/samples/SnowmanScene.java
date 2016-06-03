package ru.vlitomsk.raytr.geom.samples;

import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.scenic.Light;
import ru.vlitomsk.raytr.geom.scenic.Scene;
import ru.vlitomsk.raytr.geom.scenic.SceneObject;
import ru.vlitomsk.raytr.geom.scenic.*;

import java.awt.*;

public class SnowmanScene implements Scene {
    private SceneObject plane, wall1, wall2, wall3, box1, sph1, sph2, sph3;

    private SceneObject[] sceneObjects;

    @Override
    public SceneObject[] getSceneObjects() {
        return sceneObjects;
    }

    private Color diffColor = new Color(255, 255, 255);
    @Override
    public Color getDiffuseLightColor() {
        return diffColor;
    }

    @Override
    public Light[] getLights() {
        return new Light[]{
                new Light(new Vec(100, 1, 40), new Color(255, 255, 200)),
                //new Light(new Vec(-3, 0.5, 1), new Color(100, 255, 100)),
                //new Light(new Vec(-3, -5, 6), new Color(100, 100, 255)),
                new Light(new Vec(0, 20, 8), new Color(200,255,255)),
                new Light(new Vec(0, -60, 8), new Color(255,200,255))
        };
    }

    private Vec color2vec(Color col) {
        Vec v = new Vec();
        v.comp[0] = col.getRed();
        v.comp[1] = col.getGreen();
        v.comp[2] = col.getBlue();
        v.comp[3] = 0;
        return v;
    }

    public SnowmanScene() {
        plane = new QuadrangleSceneObject(new Vec(-15,-15), new Vec(-15, 15), new Vec(15, 15), new Vec(15, -15));
        wall1 = new QuadrangleSceneObject(new Vec(-15, 0, 0), new Vec(0, -15, 0), new Vec(0, -15, 20), new Vec(-15, 0, 20));
        wall2 = new QuadrangleSceneObject(new Vec(30, -15, -1), new Vec(30, -15, 20), new Vec(-30, -15, 20), new Vec(-30, -15, -1));
        sph1 = new SphereSceneObject(new Vec(0, 0, 4), 4);
        sph2 = new SphereSceneObject(new Vec(0, 0, 10), 3);
        sph3 = new SphereSceneObject(new Vec(0, 0, 14),2);

        double coef = 1.0/255.0;
        plane.getOpticParams().coefDiffuse = color2vec(Color.darkGray).pcoef3(coef);
        wall1.getOpticParams().coefDiffuse = color2vec(Color.lightGray).pcoef3(coef);
        wall2.getOpticParams().coefDiffuse = color2vec(new Color(255, 255, 0)).pcoef3(coef);
        sph1.getOpticParams().coefDiffuse = color2vec(Color.pink).pcoef3(coef);
        wall2.getOpticParams().power = 100000000;
        plane.getOpticParams().power = 500;
        sph1.getOpticParams().power = 300;
        sph2.getOpticParams().coefDiffuse = color2vec(Color.magenta).pcoef3(coef);
        sph2.getOpticParams().power = 400;
        sph3.getOpticParams().coefDiffuse = color2vec(Color.cyan).pcoef3(coef);
        sph3.getOpticParams().power = 500;

        sceneObjects = new SceneObject[] {plane, sph1, sph2, sph3, wall2};
    }
}
