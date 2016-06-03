package ru.nsu.ccfit.litvinov.cg.isolines;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by v on 4/5/16.
 */
public class Controller {
    private GridFunctionModel gridFunctionModel;
    private XYFunction fn;
    private ViewSettings vs;


    public Controller(ViewSettings vs, GridFunctionModel gridFunctionModel) {
        this.vs = vs;
        this.gridFunctionModel = gridFunctionModel;
    }

    public void newSettings(ModelSettings res) {
        gridFunctionModel.newSettings(res);
    }

    public void readSettings(String fpath) throws IOException {
        FileInputStream fis = new FileInputStream(fpath);
        Settings sett = Settings.readSettings(fis);
        gridFunctionModel.newSettings(sett.ms);
        fis.close();
    }

    public void writeSettings(String fpath) throws IOException {
        FileOutputStream fos = new FileOutputStream(fpath);
        Settings.writeSettings(gridFunctionModel.getModelSettings(), vs, fos);
        fos.close();
    }
}
