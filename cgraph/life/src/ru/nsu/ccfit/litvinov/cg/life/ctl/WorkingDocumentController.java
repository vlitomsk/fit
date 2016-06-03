package ru.nsu.ccfit.litvinov.cg.life.ctl;

import ru.nsu.ccfit.litvinov.cg.life.model.*;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Observer;

/**
 * Created by vas on 16.02.16.
 */
public class WorkingDocumentController {
    WorkingDocument wdoc;
    FieldController ctl;

    public FieldController getFieldCtl() {
        return ctl;
    }

    public WorkingDocumentController(WorkingDocument wdoc) {
        this.wdoc = wdoc;
    }

    public void newDoc(int mSize, int nSize, int height, int thickness) {
        Field field = new Field(mSize, nSize, Rules.DEFAULT);
        ctl = new FieldController(field);
        wdoc.setDoc(new Document(field, new FieldDisplayProps(height, thickness)));
    }

    public void saveDoc(String path) throws IOException {
        wdoc.getDoc().save(new FileOutputStream(path));
    }

    public void openDoc(String path) throws IOException, BadFileFormatException {
        wdoc.setDoc(Document.open(new FileInputStream(path)));
        ctl = new FieldController(wdoc.getDoc().getField());
    }

    public WorkingDocument getWdoc() {
        return wdoc;
    }

    public void subscribe(Observer observer) {
        wdoc.addObserver(observer);
    }
}
