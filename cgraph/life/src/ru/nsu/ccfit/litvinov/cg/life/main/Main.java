package ru.nsu.ccfit.litvinov.cg.life.main;

import ru.nsu.ccfit.litvinov.cg.life.ctl.FieldController;
import ru.nsu.ccfit.litvinov.cg.life.ctl.WorkingDocumentController;
import ru.nsu.ccfit.litvinov.cg.life.model.Field;
import ru.nsu.ccfit.litvinov.cg.life.model.Rules;
import ru.nsu.ccfit.litvinov.cg.life.model.WorkingDocument;
import ru.nsu.ccfit.litvinov.cg.life.view.MainWindow;

import javax.swing.*;

public class Main {

    public static void main(String[] args) throws InterruptedException {

        int mSize = 40;
        int nSize = 40;
        Field field = new Field(mSize, nSize, Rules.DEFAULT);

        FieldController ctl = new FieldController(field);
       // SwingUtilities.invokeLater(() -> new MainWindow(field, ctl));
        //EditorState editorState = new EditorState();

        WorkingDocument wdoc = new WorkingDocument();
        WorkingDocumentController workingDocumentController = new WorkingDocumentController(wdoc);
        SwingUtilities.invokeLater(() -> {
            MainWindow mainWindow = new MainWindow(workingDocumentController);
        });
    }
}
