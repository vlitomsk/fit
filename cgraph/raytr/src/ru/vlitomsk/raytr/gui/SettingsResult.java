package ru.vlitomsk.raytr.gui;

import java.awt.*;

/**
 * Created by vas on 31.05.2016.
 */
class SettingsResult {
    public Color bgColor;
    public double gamma;
    public int depth, quality;

    public SettingsResult(Color bgColor, double gamma, int depth, int quality) {
        this.bgColor = bgColor;
        this.gamma = gamma;
        this.depth = depth;
        this.quality = quality;
    }
}
