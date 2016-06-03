package ru.nsu.ccfit.litvinov.cg.filter;

import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BadBMPFormatException;
import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BitmapImage;

import java.awt.image.BufferedImage;
import java.io.*;

public class Controller {
    public BitmapImage bmp;
    public Model model;
    public void openDoc(String path) throws IOException,BadBMPFormatException{
        bmp = BitmapImage.load(new FileInputStream(path));
        model.upd();
    }

    public void saveDoc(String path, BufferedImage img) throws IOException {
        BitmapImage img_bmp = new BitmapImage(img);
        BitmapImage.save(img_bmp, new FileOutputStream(path));
        //model.upd();
    }
}
