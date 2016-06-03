package ru.nsu.ccfit.litvinov.cg.life.model;

import java.util.Observable;
import java.util.Observer;

/**
 * Created by vas on 16.02.16.
 */
public class MidObservable extends Observable implements Observer {
    @Override
    public void update(Observable o, Object arg) {
        pass(arg);
    }

    void pass(Object arg) {
        setChanged();
        notifyObservers(arg);
    }
}
