package ru.nsu.ccfit.litvinov.cg.filter;

import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;
import ru.nsu.ccfit.litvinov.cg.filter.bmpformat.BadBMPFormatException;

import javax.swing.*;
import javax.swing.border.LineBorder;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.*;

public class MainWindow extends JFrame implements  Observer {
    private static final String appTitle = "filter lab";

    private static final String
            ICON_APP_EXIT = "application-exit.png",
            ICON_NEW_DOCUMENT = "document-new.png",
            ICON_OPEN_DOCUMENT = "document-open.png",
            ICON_SAVE_DOCUMENT = "document-save.png",
            ICON_HELP_ABOUT = "help-about.png";

    private static final String
            SACT_EXIT = "Exit",
            SACT_NEWDOC = "New",
            SACT_SAVEDOC = "Save",
            SACT_OPENDOC = "Open",
            SACT_GREY = "Greyscale",
            SACT_NEG = "Negative",
            SACT_STEIN = "Steinberg",
            SACT_ORDER = "Ordered",
            SACT_DBL = "Double",
            SACT_DIFF = "Sobel",
            SACT_EDGE = "Edges",
            SACT_BLUR = "Blur",
            SACT_REZKO = "Rezko",
            SACT_TISN = "Tisn",
            SACT_AQUA = "Aqua",
            SACT_ROT = "Rotate",
            SACT_GAMMA = "Gamma",
            SACT_ABOUT = "About";

    @Override
    public void update(Observable o, Object arg) {
        if (o == null && arg == null) {
            rep1();
        }
    }

    BufferedImage img1;
    BufferedImage img2 = null;
    BufferedImage img3 = null;

    private void resz_labels() {
        int one_third = (getWidth()-8) / 3;
        img_sz = img_h = one_third - 8;
        Dimension dim = new Dimension(img_sz, img_sz);
        JLabel[] lbls = {lbl1,lbl2,lbl3};
        int i = 0;
        for (JLabel lbl : lbls) {
            set_lbl_size(lbl, dim);
            lbl.setLocation(one_third * i + 4, 8);
            ++i;
        }
        clip_sz = img_sz / 4;
        set_lbl_size(lblclip, clip_sz);
        lblclip.setLocation(lbl1.getLocation());
        rep1();
    }

    private void move_clip(int cx, int cy) {
        int rx = satur(cx - clip_sz / 2, 0, img_sz - clip_sz);
        int ry = satur(cy - clip_sz / 2, 0, img_h - clip_sz);
        lblclip.setLocation(rx + lbl1.getX(), ry + lbl1.getY());
        rep2();
    }

    void rep1() {
        BufferedImage origImg = null;
        if (ctl.bmp != null)
            origImg = ctl.bmp.getImg();
        else
            return;

        if (origImg.getWidth() > img_sz) {
            img1 = VarSizeGraphicAlgo.scale(origImg, img_sz * 1.0 / origImg.getWidth());
            img_h = img1.getHeight();
        } else {
            img1 = ConstSizeGraphicAlgo.deepCopy(origImg);
        }
        lbl1.setIcon(new ImageIcon(img1));
        rep2();
    }

    void rep2() {
        if (img1 == null)
            return;
        BufferedImage orig = ctl.bmp.getImg();
        int coef = (int)Math.floor(orig.getWidth() / (double)img_sz);
        int sz = (int)Math.floor((double)orig.getWidth() * clip_sz / (double)img_sz);
        BufferedImage tmp = orig.getSubimage(
                (lblclip.getX() - lbl1.getX()) * coef ,
                (lblclip.getY() - lbl1.getY()) * coef,
                sz,sz);
        img2 = VarSizeGraphicAlgo.scale(tmp, img_sz / (double)tmp.getWidth());
        lbl2.setIcon(new ImageIcon(img2));
        rep3();
    }

    BufferedImage filt = null;
    BufferedImage img4 = null;
    void rep3() {
        if (img1 == null || filt == null)
            return;

        if (img3 == null) {
            BufferedImage orig = ctl.bmp.getImg();
            int coef = (int) Math.floor(orig.getWidth() / (double) img_sz);
            int sz = (int) Math.floor((double) orig.getWidth() * clip_sz / (double) img_sz);
            BufferedImage tmp = filt.getSubimage(
                    (lblclip.getX() - lbl1.getX()) * coef,
                    (lblclip.getY() - lbl1.getY()) * coef,
                    sz, sz);
            img4 = VarSizeGraphicAlgo.scale(tmp, img_sz / (double) tmp.getWidth());
            lbl3.setIcon(new ImageIcon(img4));
        } else {
            lbl3.setIcon(new ImageIcon(img3));
        }
    }

    private void dc() {
        filt = ConstSizeGraphicAlgo.deepCopy(ctl.bmp.getImg());
    }

    private boolean copen() {
        if (img2 == null) {
            JOptionPane.showMessageDialog(MainWindow.this, "Open image first",
                    "Error", JOptionPane.ERROR_MESSAGE);
            return true;
        }
        return false;
    }

    ActionListener greyListener = (e -> { img3 = null; if (copen()) return;dc(); ConstSizeGraphicAlgo.desaturate(filt); rep3(); });
    ActionListener negListener = (e -> { img3 = null; if (copen()) return;dc();ConstSizeGraphicAlgo.negate(filt); rep3(); });
    ActionListener steinListener = (e -> {
        if (copen()) return;
        int[] mins = {2, 2, 2};
        int[] maxs = {30, 30, 30};
        String[] labels = {"Nr ", "Ng ", "Nb "};
        IntDialog dlg = new IntDialog(this, "Steinberg dithering", 3, mins, maxs, mins, labels);
        int[] res;
        if ((res = dlg.getResult()) != null) {
            img3 = null;
            dc();
            ConstSizeGraphicAlgo.steinbergDither(filt, res[0], res[1], res[2]);
            rep3();
        }
    });
    ActionListener orderListener = (e -> {
        if (copen()) return;
        int[] mins = {1};
        int[] maxs = {7};
        String[] labels = {"Bits "};
        IntDialog dlg = new IntDialog(this, "Ordered dithering", 1, mins, maxs, mins, labels);
        int[] res;
        if ((res = dlg.getResult()) != null) {
            img3 = null;
            dc();
            ConstSizeGraphicAlgo.orderDither(filt, (~((1 << res[0]) - 1)) & 0xff);
            rep3();
        }
    });

    ActionListener edgeListener = (e -> {
        if (copen()) return;
        int[] mins = {1};
        int[] maxs = {255};
        String[] labels = {"Threshold "};
        IntDialog dlg = new IntDialog(this, "Edge detect", 1, mins, maxs, mins, labels);
        int[] res;
        if ((res = dlg.getResult()) != null) {
            img3 = null;
            dc();
            ConstSizeGraphicAlgo.desaturate(filt);
            ConstSizeGraphicAlgo.blur(filt);
            ConstSizeGraphicAlgo.magnitude(filt);
            ConstSizeGraphicAlgo.morethan(filt, res[0], res[0], res[0], true);
            rep3();
        }
    });
    ActionListener diffListener = (e -> { img3 = null; if (copen()) return;dc();ConstSizeGraphicAlgo.magnitude(filt); rep3(); });
    ActionListener blurListener = (e -> { img3 = null; if (copen()) return;dc();ConstSizeGraphicAlgo.blur(filt); rep3(); });
    ActionListener rezkoListener = (e -> { img3 = null; if (copen()) return;dc();ConstSizeGraphicAlgo.sharpen(filt); rep3(); });
    ActionListener aquaListener = (e -> { img3 = null; if (copen()) return;dc();ConstSizeGraphicAlgo.aqua(filt); rep3(); });
    ActionListener gammaListener = (e -> {
        img3 = null;
        if (copen()) return;
        dc();
        ConstSizeGraphicAlgo.gammaCorr(filt, 1.2);
        rep3();
    });

    ActionListener dblListener = (e -> {
        if (copen()) return;
        dc();
        BufferedImage toscale = img2.getSubimage(0, 0, img2.getWidth() / 2, img2.getHeight() / 2);
        img3 = VarSizeGraphicAlgo.scale(toscale, 2.0);
        rep3();
    });

    ActionListener rotListener = (e -> {
        if (copen()) return;
        dc();
        int[] mins = {1};
        int[] maxs = {90};
        String[] labels = {"Angle "};
        IntDialog dlg = new IntDialog(this, "Rotate", 1, mins, maxs, mins, labels);
        int[] res;
        if ((res = dlg.getResult()) != null) {
            BufferedImage rot = VarSizeGraphicAlgo.rotate(img2, Math.PI / 6.0);
            img3 = VarSizeGraphicAlgo.scale(rot, ((double)img2.getWidth()) / (double)rot.getWidth());
            rep3();
        }
    });

    private static final String
            MENU_FILE = "File",
            MENU_FILTER = "Filter",
            MENU_HELP = "Help";

    private static final String aboutString =
            "This is filter lab\n" +
            "by Vasiliy Litvinov <kvas.omsk at gmail.com>";


    Controller ctl;
    JLabel lbl1, lbl2, lbl3, lblclip;
    int img_sz, clip_sz, img_h;

    private void set_lbl_size(JLabel lbl, Dimension dim) {
        lbl.setMinimumSize(dim);
        lbl.setPreferredSize(dim);
        lbl.setSize(dim);
    }

    private void set_lbl_size(JLabel lbl, int sz) {
        set_lbl_size(lbl, new Dimension(sz, sz));
    }

    private int satur(int x, int a, int b) {
        if (x <= a)
            return a;
        if (x >= b)
            return b;
        return x;
    }

    public MainWindow(Controller ctl) {
        this.ctl = ctl;
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle(appTitle);
        Dimension minSz = new Dimension(800,600);
        setSize(minSz);
        setMinimumSize(minSz);

        addMenu();

        JPanel panel = new JPanel();
        panel.setLayout(null);
        panel.add(lbl1 = new JLabel());
        lbl1.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                super.mousePressed(e);
                move_clip(e.getX(), e.getY());
            }
        });
        lbl1.addMouseMotionListener(new MouseAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                super.mouseDragged(e);
                move_clip(e.getX(), e.getY());
            }
        });
        panel.add(lbl2 = new JLabel());
        panel.add(lbl3 = new JLabel());

        addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                super.componentResized(e);
                resz_labels();
            }
        });
        lblclip = new JLabel();
        panel.add(lblclip);

        JLabel[] lbls = {lblclip, lbl1,lbl2,lbl3};
        for (JLabel l : lbls) {
            panel.setComponentZOrder (l, l != lblclip ? 2 : 1);
            if (l != lblclip)
                l.setBorder(new LineBorder(Color.BLACK));
            l.setVerticalAlignment(JLabel.TOP);
            l.setVerticalTextPosition(JLabel.TOP);
        }
        lblclip.setOpaque(true);
        lblclip.setBackground(new Color(255, 255, 255, 30));

        resz_labels();

        add(panel);

        setLocationRelativeTo(null);
        setVisible(true);
    }

    private ActionListener killListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            System.exit(0);
        }
    };

    JFileChooser fchooser = new JFileChooser();
    private ActionListener openDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showOpenDialog(MainWindow.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        ctl.openDoc(fl.getCanonicalPath());
                        img4 = img3 = filt = null;
                        lbl3.setIcon(null);
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, ex.toString(),
                                "IO exception", JOptionPane.ERROR_MESSAGE);
                    } catch (BadBMPFormatException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, ex.getMessage(),
                                "Bad file format", JOptionPane.ERROR_MESSAGE);
                    }
                }
            });
        }

    };

    private ActionListener saveDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                if (img4 == null && img3 == null)
                    return;
                int rv = fchooser.showSaveDialog(MainWindow.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        ctl.saveDoc(fl.getCanonicalPath(), img3 != null ? img3 : img4);
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, "IOException while opening!");
                    }
                }
            });
        }
    };

    private ActionListener aboutListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(MainWindow.this, aboutString));
        }
    };

    private class ItemDesc {
        String title;
        ImageIcon icon;
        ActionListener listener;

        public ItemDesc(String title, String iconPath, ActionListener listener) {
            if (iconPath != null)
                this.icon = new ImageIcon(iconPath);
            else
                this.icon = null;
            this.listener = listener;
            this.title = title;
        }
    }

    private final Map<String, ItemDesc> mItems = new HashMap<String,ItemDesc>() {
        {
            put(SACT_EXIT, new ItemDesc(SACT_EXIT, null, killListener));
            put(SACT_OPENDOC, new ItemDesc(SACT_OPENDOC, null, openDocListener));
            put(SACT_SAVEDOC, new ItemDesc(SACT_SAVEDOC, null, saveDocListener));
            put(SACT_ABOUT, new ItemDesc(SACT_ABOUT, null, aboutListener));
            put(SACT_GREY, new ItemDesc(SACT_GREY, null, greyListener));
            put(SACT_NEG, new ItemDesc(SACT_NEG, null, negListener));
            put(SACT_STEIN, new ItemDesc(SACT_STEIN, null, steinListener));
            put(SACT_ORDER, new ItemDesc(SACT_ORDER, null, orderListener));
            put(SACT_DBL, new ItemDesc(SACT_DBL, null, dblListener));
            put(SACT_DIFF, new ItemDesc(SACT_DIFF, null, diffListener));
            put(SACT_EDGE, new ItemDesc(SACT_EDGE, null, edgeListener));
            put(SACT_BLUR, new ItemDesc(SACT_BLUR, null, blurListener));
            put(SACT_REZKO, new ItemDesc(SACT_REZKO, null, rezkoListener));
            put(SACT_AQUA, new ItemDesc(SACT_AQUA, null, aquaListener));
            put(SACT_ROT, new ItemDesc(SACT_ROT, null, rotListener));
            put(SACT_GAMMA, new ItemDesc(SACT_GAMMA, null, gammaListener));
        }
    };

    private void addMenuItem(JMenu menu, String itemName) {
        ItemDesc desc = mItems.get(itemName);
        JMenuItem mitem = new JMenuItem(desc.title);
        mitem.addActionListener(desc.listener);
        if (desc.icon != null)
            mitem.setIcon(desc.icon);
        menu.add(mitem);
    }

    private void addToolbarBtn(JToolBar tb, String itemName) {
        ItemDesc desc = mItems.get(itemName);
        JButton btn = new JButton();
        btn.setIcon(desc.icon);
        btn.addActionListener(desc.listener);
        btn.setToolTipText(desc.title);
        tb.add(btn);
    }

    private JMenu createFileMenu() {
        JMenu fileMenu = new JMenu(MENU_FILE);

        addMenuItem(fileMenu, SACT_SAVEDOC);
        addMenuItem(fileMenu, SACT_OPENDOC);
        addMenuItem(fileMenu, SACT_EXIT);

        return fileMenu;
    }

    private JMenu createHelpMenu() {
        JMenu helpMenu = new JMenu(MENU_HELP);

        addMenuItem(helpMenu, SACT_ABOUT);

        return helpMenu;
    }

    private JMenu createFilterMenu() {
        JMenu filterMenu = new JMenu(MENU_FILTER);

        addMenuItem(filterMenu, SACT_GREY);
        addMenuItem(filterMenu, SACT_NEG);
        addMenuItem(filterMenu, SACT_STEIN);
        addMenuItem(filterMenu, SACT_ORDER);
        addMenuItem(filterMenu, SACT_DBL);
        addMenuItem(filterMenu, SACT_DIFF);
        addMenuItem(filterMenu, SACT_EDGE);
        addMenuItem(filterMenu, SACT_BLUR);
        addMenuItem(filterMenu, SACT_REZKO);
        addMenuItem(filterMenu, SACT_AQUA);
        addMenuItem(filterMenu, SACT_ROT);
        addMenuItem(filterMenu, SACT_GAMMA);

        return filterMenu;
    }

    private void addMenu() {
        JMenuBar menu = new JMenuBar();

        menu.add(createFileMenu());
        menu.add(createFilterMenu());
        menu.add(createHelpMenu());

        setJMenuBar(menu);
    }
}
