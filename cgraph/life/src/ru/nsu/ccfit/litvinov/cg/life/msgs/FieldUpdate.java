package ru.nsu.ccfit.litvinov.cg.life.msgs;

import ru.nsu.ccfit.litvinov.cg.life.model.Field;

/**
 * Created by vas on 16.02.16.
 */
public class FieldUpdate {
    public Field fld;

    public FieldUpdate(Field fld) {
        this.fld = fld;
    }
}
