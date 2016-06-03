package ru.nsu.ccfit.litvinov.cg.wireframe.show;

import javax.swing.*;
import java.awt.*;

/**
 * Created by v on 4/19/16.
 */
public class JUtils {
    public static void resize(Component wnd, int w, int h) {
        Dimension dim = new Dimension(w, h);
        wnd.setSize(dim);
        wnd.setMinimumSize(dim);
        wnd.setPreferredSize(dim);
    }
}
