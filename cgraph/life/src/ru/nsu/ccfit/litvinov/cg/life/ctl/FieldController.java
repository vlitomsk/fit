package ru.nsu.ccfit.litvinov.cg.life.ctl;

import ru.nsu.ccfit.litvinov.cg.life.model.Field;

/**
 * Created by vas on 15.02.16.
 */
public class FieldController {
    private Field field;

    private enum SelectMode {
        DISABLED,
        SELECT_NOT,
        SELECT_SET
    }

    private SelectMode selectMode = SelectMode.SELECT_NOT;
    private int actionNum = 1;
    boolean actionStarted = false;
    private int[][] actionMarks ;

    public void setXorMode() {
        selectMode = SelectMode.SELECT_NOT;
    }

    public void setSetMode() {
        selectMode = SelectMode.SELECT_SET;
    }

    public FieldController(Field field) {
        this.field = field;
        actionMarks = new int[field.getmSize()][field.getnSize()];
    }

    public void finishAction() {
        actionNum++;
        actionStarted = false;
    }

    public void contAction(int i, int j) {
        if (!actionStarted) {
            return ;
        }
        if (actionMarks[i][j] == actionNum) {
            return;
        }
        //System.out.println("action " + i + " " + j);
        actionMarks[i][j] = actionNum;

        SelectMode selectMode;
        synchronized (this.selectMode) {
            selectMode = this.selectMode;
        }

        switch (selectMode) {
            case DISABLED:
                return;
            case SELECT_SET:
                field.set(i, j);
                break;
            case SELECT_NOT:
                field.not(i, j);
                break;
            default:
                throw new IllegalArgumentException("this mode isn't implemented");
        }
    }

    public void startAction() {
        actionStarted = true;
        //System.out.println("start");
    }

    public void mousePressed(int i, int j) {
        startAction();
        contAction(i, j);
    }

    public void mouseDragged(int i, int j) {
        //System.out.println("drag " + i + ' ' + j);
        contAction(i, j);
    }

    public void mouseReleased(int i, int j) {
        finishAction();
    }

    public void mouseEntered(int i, int j) {
        // stub
    }

    public void mouseMoved(int i, int j) {
        //System.out.println("moved " + i + " " +j);
        contAction(i, j);
    }

    public void mouseExited(int i, int j) {
        // stub
        finishAction();
    }
}
