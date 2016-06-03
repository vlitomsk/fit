package ru.nsu.ccfit.litvinov.cg.filter;

import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BitmapImage;

import java.util.*;

public class Model {
    public Observer obs;
    public void upd() {
        obs.update(null,null);
    }
}
