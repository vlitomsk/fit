package ru.nsu.ccfit.litvinov.cg.wireframe.dev;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class DragPane extends JPanel {
    class IdTuple extends MouseMotionAdapter {
        private IdTuple prev, next;
        private JComponent jc;
        double rx, ry;
        int id;

        public IdTuple(int id, JComponent jc, double rx, double ry) {
            this.id = id;
            this.jc = jc;
            this.rx = rx;
            this.ry = ry;
            jc.addMouseMotionListener(this);
        }

        public void setSingle() {
            prev = next = null;
            draggableList = this;
        }

        private int getPrevX() {
            return prev.jc.getX();
        }

        private int getNextX() {
            return next.jc.getX();
        }

        void plist() {
            IdTuple tpl = draggableList;
            while (tpl != null) {
                System.out.print(tpl.jc.getX() + " -> ");
                tpl = tpl.next;
            }
            System.out.println("()");
        }

        public void insert(IdTuple tpl) {
            tpl.prev = null;
            tpl.next = draggableList;
            draggableList = tpl;
            ++draggableListSize;

            relocate(draggableList);
            plist();
        }

        private void swap(IdTuple tpl1, IdTuple tpl2) {
            IdTuple n = tpl2.next;
            IdTuple p = tpl2.prev;

            tpl2.next = tpl1.next;
            tpl2.prev = tpl1.prev;

            tpl1.next = n;
            tpl1.prev = p;

            checkHead(tpl1);
            checkHead(tpl2);
        }

        private void checkHead(IdTuple tpl) {
            if (tpl.prev == null)
                draggableList = tpl;
        }

        public JComponent removeById(int id) {
            IdTuple tpl = draggableList;
            while (tpl != null) {
                if (tpl.id == id) {
                    --draggableListSize;

                    if (tpl.prev != null)
                        tpl.prev.next = tpl.next;
                    else
                        draggableList = tpl.next;

                    if (tpl.next != null)
                        tpl.next.prev = tpl.prev;

                    jc.removeMouseMotionListener(this);

                    return jc;
                }
                tpl = tpl.next;
            }
            return null;
        }

        private void relocate(IdTuple tpl) {
            if (draggableListSize < 2)
                return;

            int x = tpl.jc.getX();
            while (tpl.prev != null && x < getPrevX()) {
                System.out.println("relocating --");
                swap(tpl, prev);
            }

            while (next != null && x > getNextX()) {
                System.out.println("relocating ++");
                swap(tpl, next);
            }
        }

        private void relocate() {
            relocate(this);
        }

        public void updateLocationWithoutReorder() {
            jc.setLocation(mapx(rx), mapy(ry));
        }

        @Override
        public void mouseDragged(MouseEvent e) {
            //super.mouseDragged(e);
            rx = unmapx(filterX(jc.getX() + e.getX()));
            ry = unmapy(filterY(jc.getY() + e.getY()));
            updateLocationWithoutReorder();
            synchronized (draggableList) {
                relocate();
            }
        }
    }

    private IdTuple draggableList;
    private int draggableListSize = 0;

    private double domx1, domx2, domy1, domy2;
    private double mapCoef, mapCoef_1;

    ComponentAdapter cadapt = new ComponentAdapter() {
        @Override
        public void componentResized(ComponentEvent e) {
            if (e != null)
                super.componentResized(e);
            double domx = domx2 - domx1;
            double domy = domy2 - domy1;
            //if (domx / domy >= ((double)getWidth()) / ((double)getHeight()))
            if (domx * getHeight() >= domy * getWidth())
                mapCoef = getWidth() / domx;
            else
                mapCoef = getHeight() / domy;
            mapCoef_1 = 1.0 / mapCoef;

            IdTuple tpl = draggableList;
            while (tpl != null) {
                tpl.updateLocationWithoutReorder();
            }
        }
    };

    public DragPane(double domx1, double domx2, double domy1, double domy2) {
        super();
        setLayout(null);
        if (domx1 == domx2)
            throw new RuntimeException("domx1 == domx2");
        if (domy1 == domy2)
            throw new RuntimeException("domy1 == domy2");
        this.domx1 = Math.min(domx1, domx2);
        this.domx2 = Math.max(domx1, domx2);
        this.domy1 = Math.min(domy1, domy2);
        this.domy2 = Math.max(domy1, domy2);

        cadapt.componentResized(null);

        addComponentListener(cadapt);
    }

    @Override
    public void setPreferredSize(Dimension preferredSize) {
        super.setPreferredSize(preferredSize);
        cadapt.componentResized(null);
    }

    @Override
    public void setSize(Dimension s) {
        super.setSize(s);
        cadapt.componentResized(null);
    }

    @Override
    public void setMinimumSize(Dimension d) {
        super.setMinimumSize(d);
        cadapt.componentResized(null);
    }

    private int mapx(double x) {
        return (int) Math.round((x - domx1) * mapCoef);
    }

    private int mapy(double y) {
        return (int) Math.round((y - domy1) * mapCoef);
    }

    public double unmapx(int x) {
        return ((double)x) * mapCoef_1 + domx1;
    }

    public double unmapy(int y) {
        return ((double)y) * mapCoef_1 + domy1;
    }

    private int satur(int val, int low, int hi) {
        if (val < low)
            return low;
        if (val > hi)
            return hi;
        return val;
    }

    private int filterX(int x) {
        return satur(x, 0, getWidth() - 1);
    }

    private int filterY(int y) {
        return satur(y, 0, getHeight() - 1);
    }

    public void addDraggable(int id, JComponent d, double x, double y) {
        IdTuple tpl = new IdTuple(id, d, x, y);
        if (draggableList == null) {
            tpl.setSingle();
        } else {
            synchronized (draggableList) {
                draggableList.insert(tpl);
            }
        }
        tpl.updateLocationWithoutReorder();
        add(d);
    }

    private void removeDraggable(int id) {
        JComponent jc = null;
        synchronized (draggableList) {
            jc = draggableList.removeById(id);
        }
        if (jc != null)
            remove(jc);
    }

    double[][] coordsRes = null;
    public double[][] getCoords() {
        synchronized (draggableList) {
            if (coordsRes == null || coordsRes.length != draggableListSize) {
                coordsRes = new double[draggableListSize][2];
            }
            int i = 0;
            IdTuple tpl = draggableList;
            while (tpl != null) {
                coordsRes[i][0] = unmapx(tpl.jc.getX());
                coordsRes[i][1] = unmapy(tpl.jc.getY());
                ++i;
                tpl = tpl.next;
            }
        };
        return coordsRes;
    }
}

class MainWindow extends JFrame {
    int idCounter = 0;
    public MainWindow() {
        super();
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        setTitle("Draggable");
        Dimension dim = new Dimension(800,600);
        setSize(dim);
        DragPane pane = new DragPane(-1, 1, -1, 1);
        pane.setSize(dim);
        pane.setPreferredSize(dim);
        pane.setMinimumSize(dim);
        pane.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                super.mousePressed(e);
                JLabel jb = new JLabel("a");
                Dimension d = new Dimension(20, 20);
                jb.setPreferredSize(d);;
                jb.setSize(d);
                jb.setMinimumSize(d);
                pane.addDraggable(idCounter++, jb, pane.unmapx(e.getX()), pane.unmapy(e.getY()));
                SwingUtilities.invokeLater(() -> pane.repaint());
            }
        });
        add(pane);

        setVisible(true);
    }
}

public class Main {

//    public static void main(String[] args) {
//	    SwingUtilities.invokeLater(() -> new MainWindow());
//    }
}
