package ru.nsu.ccfit.litvinov.cg.isolines;

import java.io.*;

public class Main {

    private static final XYFunction defaultFn = (x, y) -> {
        double r1 = Math.sqrt(x*x + y*y);
        //return Math.exp(-r1);
        double r2 = Math.sqrt((x - 0.4)*(x-0.4) + (y-0.7)*(y-0.7));
        double r3 = Math.sqrt((x - 1)*(x-1) + (y-1)*(y+1));
        return ((r1 < 0.1) ? 1 : Math.exp(-(r1 - 0.1) * 2)) +
               ((r2 < 0.1) ? 1 : Math.exp(-(r2 - 0.1) * 2)) +
                Math.exp(x);

    };


    public static void main(String[] args) throws IOException {
	// write your code here
        FileInputStream fis = new FileInputStream("default.props");
        Settings settings = Settings.readSettings(fis);
        settings.ms.a = -3;
        settings.ms.b = 3;
        settings.ms.c = -3;
        settings.ms.d = 3;

        GridFunctionModel gfm = new GridFunctionModel(defaultFn, settings.ms);
        Controller ctl = new Controller(settings.vs, gfm);
        MainWindow mw = new MainWindow(ctl, settings.vs);
        gfm.addObserver(mw);
        ctl.newSettings(settings.ms);
    }
}
