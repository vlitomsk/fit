package ru.nsu.ccfit.litvinov.cg.life.model;

import ru.nsu.ccfit.litvinov.cg.life.msgs.FieldDisplayPropsUpdate;

import java.util.Observable;
import java.util.Observer;

/**
 * Created by vas on 16.02.16.
 */
public class FieldDisplayProps extends Observable {
    private int lineThickness;
    private int cellHeight;

    public FieldDisplayProps(int cellHeight, int lineThickness) {
        this.cellHeight = cellHeight;
        this.lineThickness = lineThickness;
    }

    public int getCellHeight() {
        return cellHeight;
    }

    public int getLineThickness() {
        return lineThickness;
    }

    public void setCellHeight(int cellHeight) {
        this.cellHeight = cellHeight;
        setChanged();
        notifyObservers(new FieldDisplayPropsUpdate(this));
    }

    public void setLineThickness(int lineThickness) {
        this.lineThickness = lineThickness;
        setChanged();
        notifyObservers(new FieldDisplayPropsUpdate(this));
    }


}
