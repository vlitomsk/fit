package ru.nsu.ccfit.litvinov.cg.life.view;

import ru.nsu.ccfit.litvinov.cg.life.ctl.FieldController;
import ru.nsu.ccfit.litvinov.cg.life.ctl.WorkingDocumentController;
import ru.nsu.ccfit.litvinov.cg.life.model.Field;
import ru.nsu.ccfit.litvinov.cg.life.model.FieldDisplayProps;
import ru.nsu.ccfit.litvinov.cg.life.model.GraphicAlgo;
import ru.nsu.ccfit.litvinov.cg.life.model.WorkingDocument;
import ru.nsu.ccfit.litvinov.cg.life.msgs.FieldDisplayPropsUpdate;
import ru.nsu.ccfit.litvinov.cg.life.msgs.FieldUpdate;
import ru.nsu.ccfit.litvinov.cg.life.msgs.WorkingDocumentSet;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.util.Observable;
import java.util.Observer;
import java.util.function.BiConsumer;

public class WorkingDocumentView extends JPanel implements Observer {
    @FunctionalInterface
    interface Consumer4<A,B,C,D> {
        void accept(A a, B b, C c, D d);
    }

    class HexBounds {
        private int[] xs;
        private int[] ys;
        private int width, height;

        public HexBounds(int height) {
            regen(height);
        }

        public void regen(int height) {
            this.height = height;
            width = (height * (int)Math.round(Math.sqrt(3))) / 2;
            int[] xs = {0, width/2, width - 1, width - 1, width/2, 0, 0};
            int[] ys = {height/4, 0, height/4, 3*height/4, height - 1, 3*height/4, height/4};
            this.xs = xs;
            this.ys = ys;
        }

        public void goCyclicCW(Consumer4<Integer, Integer, Integer, Integer> cons) {
            for (int i = 0; i < xs.length - 1; ++i) {
                cons.accept(xs[i], ys[i], xs[i + 1], ys[i + 1]);
            }
        }

        public boolean isPointIn(int hexOfftX, int hexOfftY, int ptX, int ptY) {
            ptX -= hexOfftX;
            ptY -= hexOfftY;
            for (int i = 0; i < xs.length - 1; ++i) {
                int x1 = xs[i + 1] - xs[i];
                int y1 = ys[i + 1] - ys[i];
                int x2 = ptX - xs[i];
                int y2 = ptY - ys[i];
                if (x1 * y2 - x2 * y1 < 0)
                    return false;
            }
            return true;
        }

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }
    }

    boolean pressState = false;

    class ViewMouseAdapter extends MouseAdapter {
        @Override
        public void mousePressed(MouseEvent e) {
            if (wctl == null || wctl.getFieldCtl() == null)
                return;
            pressState = true;
            (wctl.getFieldCtl()).startAction();
            preciseEvtInvoke(e, (wctl.getFieldCtl())::contAction);
        }

        @Override
        public void mouseDragged(MouseEvent e) {
            if (pressState) {
                preciseEvtInvoke(e, (wctl.getFieldCtl())::contAction);
            }
        }

        @Override
        public void mouseReleased(MouseEvent e) {
            if (wctl == null || wctl.getFieldCtl() == null)
                return;
            pressState = false;
            (wctl.getFieldCtl()).finishAction();
        }

        private void preciseEvtInvoke(MouseEvent e, BiConsumer<Integer, Integer> cons) {
            if (e.getX() >= 0 && e.getY() >= 0 && e.getX() < totalImage.getWidth() && e.getY() < totalImage.getHeight()) {
                int rgb = totalImage.getRGB(e.getX(), e.getY());
                if (rgb != ALIVECOLOR.getRGB() && rgb != DEADCOLOR.getRGB())
                    return;
                SwingUtilities.invokeLater(() -> {
                    //System.out.println("evt " + e.getX() + " " + e.getY());
                    for (int i = 0; i < fieldw; ++i) {
                        for (int j = 0; j < fieldh; ++j) {
                            int idx = i * fieldw + j;
                            if (hexBounds.isPointIn(hexOffsetsX[idx], hexOffsetsY[idx], e.getX(), e.getY())) {
                                //System.out.println("found " + i + " " + j);
                                cons.accept(i, j);
                            }
                        }
                    }
                });
            }
        }

    }

    private WorkingDocumentController wctl;
    private HexBounds hexBounds = null;
    private int fieldw = 0, fieldh = 0;
    private int lineThickness = 1;
    private boolean[][] field;
    private int[] hexOffsetsX;
    private int[] hexOffsetsY;

    BufferedImage aliveCellImage, deadCellImage;
    BufferedImage totalImage;
    static final Color BGCOLOR = Color.LIGHT_GRAY;
    static final Color LINECOLOR = Color.BLACK;
    static final Color ALIVECOLOR = Color.orange;
    static final Color DEADCOLOR = Color.green;

    public WorkingDocumentView(WorkingDocumentController workingDocumentController) {
        this.wctl = workingDocumentController;
        setBackground(BGCOLOR);
        ViewMouseAdapter adapter = new ViewMouseAdapter();
        addMouseListener(adapter);
        addMouseMotionListener(adapter);
        setVisible(true);
    }

    private void generateCells(int cellheight, int fieldw, int fieldh) {
        generateCell(cellheight);
        generateOffsets(fieldw, fieldh);
        Dimension dim = new Dimension(hexOffsetsX[hexOffsetsX.length - 1] + hexBounds.getWidth(),
                hexOffsetsY[hexOffsetsY.length - 1] + hexBounds.getHeight());

        //setMinimumSize(dim);
        totalImage = new BufferedImage(dim.width, dim.height, BufferedImage.TYPE_INT_ARGB);
        setSize(dim);
        setPreferredSize(dim);
        this.fieldw = fieldw;
        this.fieldh = fieldh;
    }

    private void generateOffsets(int fieldw, int fieldh) {
        if (hexOffsetsX == null || hexOffsetsX.length < fieldw * fieldh) {
            final int sz = fieldw * fieldh;
            hexOffsetsX = new int[sz];
            hexOffsetsY = new int[sz];
        }
        final int cellw = hexBounds.getWidth();
        final int cellh = hexBounds.getHeight();
        for (int i = 0; i < fieldw; ++i) {
            for (int j = 0; j < fieldh; ++j) {
                final int x0 = i * (cellw - 1) + (j & 1) * ((cellw - 1) / 2);
                int y0 = j * (cellh) * 3 / 4 ;
                if (cellh % 4 == 3)
                    y0 -= j / 4;
                if (cellh % 4 == 1) {
                    if (j % 4 > 1)
                        y0 -= j % 4 - 1;
                    y0 -= 3 * (j / 4);
                }
                hexOffsetsX[i * fieldw + j] = x0;
                hexOffsetsY[i * fieldw + j] = y0;
            }
        }
    }

    static BufferedImage deepCopy(BufferedImage bi) {
        ColorModel cm = bi.getColorModel();
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        WritableRaster raster = bi.copyData(null);
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
    }

    private void generateCell(int cellh) {
        hexBounds = new HexBounds(cellh);
        aliveCellImage = new BufferedImage(hexBounds.getWidth(), hexBounds.getHeight(), BufferedImage.TYPE_INT_ARGB);
        if (lineThickness == 1) {
            hexBounds.goCyclicCW((x0, y0, x1, y1) -> {
                GraphicAlgo.bresenhamLine(LINECOLOR, aliveCellImage, x0, y0, x1, y1);
            });
        } else {
            Graphics2D g2 = (Graphics2D) aliveCellImage.getGraphics();
            g2.setStroke(new BasicStroke(lineThickness));
            g2.setColor(LINECOLOR);
            hexBounds.goCyclicCW(g2::drawLine);
        }
        deadCellImage = deepCopy(aliveCellImage);
        final int fillx = hexBounds.getWidth() / 2;
        final int filly = hexBounds.getHeight() / 2;

        GraphicAlgo.spanFill(ALIVECOLOR, aliveCellImage, fillx, filly);
        GraphicAlgo.spanFill(DEADCOLOR, deadCellImage, fillx, filly);
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);
        if (hexBounds == null)
            return;
        Graphics2D g2d = (Graphics2D) g;
        g2d.drawImage(totalImage, 0, 0, totalImage.getWidth(), totalImage.getHeight(),
                0, 0, totalImage.getWidth(), totalImage.getHeight(), null);
    }

    private void onDocumentSet(WorkingDocumentSet upd) {
        Field fld = upd.doc.getField();
        impMat = fld.getImpMat();
        FieldDisplayProps props = upd.doc.getDprops();
        lineThickness = props.getLineThickness();
        this.field = fld.getField();
        generateCells(props.getCellHeight(), fld.getmSize(), fld.getnSize());
        drawCells(totalImage);
        SwingUtilities.invokeLater(() -> repaint());
    }

    private boolean shimpacts = false;
    public void showImpacts(boolean show) {
        shimpacts = show;
    }

    private void drawCells(BufferedImage totalImage) {
        Graphics2D g2d = (Graphics2D) totalImage.getGraphics();
        for (int i = 0; i < fieldw; ++i) {
            for (int j = 0; j < fieldh; ++j) {
                final int idx = i * fieldw + j;
                final int x0 = hexOffsetsX[idx];
                final int y0 = hexOffsetsY[idx];
                final BufferedImage imageSel = field[i][j] ? aliveCellImage : deadCellImage;
                g2d.drawImage(imageSel,
                        x0, y0, x0 + hexBounds.getWidth(), y0 + hexBounds.getHeight(),
                        0, 0, hexBounds.getWidth(), hexBounds.getHeight() , null);
                if (shimpacts && hexBounds.getHeight() > 20) {
                    String str = String.valueOf(impMat[i][j]);
                    if (str.length() > 3)
                        str = str.substring(0, 3);
                    g2d.setColor(Color.RED);
                    g2d.drawString(str, x0 + lineThickness + 4, y0 + hexBounds.getHeight() / 2 + 5);
                }
            }
        }
    }

    private double[][] impMat;

    public void forceredraw() {
        drawCells(totalImage);
        SwingUtilities.invokeLater(() -> repaint());
    }

    private void onFieldUpdate(FieldUpdate upd) {
        this.field = upd.fld.getField();
        this.impMat = upd.fld.getImpMat();
        drawCells(totalImage);
        SwingUtilities.invokeLater(() -> repaint());
    }


    private void onFieldDisplayPropsUpdate(FieldDisplayPropsUpdate upd) {
        FieldDisplayProps fdp = upd.fdp;
        lineThickness = fdp.getLineThickness();
        generateCells(fdp.getCellHeight(), this.fieldw, this.fieldh);
        drawCells(totalImage);
        SwingUtilities.invokeLater(() -> repaint());
    }

    @Override
    public void update(Observable o, Object arg) {
        if (!(o instanceof WorkingDocument))
            return;
        WorkingDocument wdoc = (WorkingDocument) o;
        if (arg instanceof WorkingDocumentSet) {
            onDocumentSet((WorkingDocumentSet) arg);
        }

        if (arg instanceof FieldUpdate) {
            onFieldUpdate((FieldUpdate) arg);
        }

        if (arg instanceof FieldDisplayPropsUpdate) {
            onFieldDisplayPropsUpdate((FieldDisplayPropsUpdate) arg);
        }
    }
}