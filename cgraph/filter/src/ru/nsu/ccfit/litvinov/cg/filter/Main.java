package ru.nsu.ccfit.litvinov.cg.filter;

import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BadBMPFormatException;
import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BitmapImage;

import java.awt.image.*;
import javax.swing.*;
import java.io.*;

public class Main extends JFrame{
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            Controller ctl = new Controller();
            Model model = new Model();
            ctl.model = model;
            MainWindow mw = new MainWindow(ctl);
            model.obs = mw;
        });
    }
}
