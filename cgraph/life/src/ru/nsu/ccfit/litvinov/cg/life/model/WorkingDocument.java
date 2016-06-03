package ru.nsu.ccfit.litvinov.cg.life.model;

import ru.nsu.ccfit.litvinov.cg.life.msgs.WorkingDocumentSet;

import java.util.Observable;
import java.util.Observer;

/**
 * Created by vas on 16.02.16.
 */
public class WorkingDocument extends MidObservable {
    Document doc;

    public Document getDoc() {
        return doc;
    }

    public void setDoc(Document doc) {
        this.doc = doc;
        doc.addObserver(this);
        doc.getField().addObserver(this);
        setChanged();
        notifyObservers(new WorkingDocumentSet(this.doc));
    }
}
