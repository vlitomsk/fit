package ru.vlitomsk.raytr.geom.scenic;

import java.awt.*;

/**
 * Created by vas on 23.05.2016.
 */
public interface Scene {
    public Color getDiffuseLightColor();
    public SceneObject[] getSceneObjects();
    public Light[] getLights();
}
