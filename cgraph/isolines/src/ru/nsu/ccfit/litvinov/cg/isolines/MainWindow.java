package ru.nsu.ccfit.litvinov.cg.isolines;

import javax.swing.*;
import javax.swing.text.Position;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

/**
 * Created by vas on 13.02.16.
 */
public class MainWindow extends JFrame implements Observer {
    private static final String appTitle = "ЛИНИИ УРОВНЯ";

    private static final String
            ICON_APP_EXIT = "application-exit.png",
            ICON_OPEN_DOCUMENT = "document-open.png",
            ICON_SAVE_DOCUMENT = "document-save.png",
            ICON_COLORS = "colors.png",
            ICON_INTERACT = "interactive.png",
            ICON_GRID = "grid.png",
            ICON_SH_ISOLINES = "isolines.png",
            ICON_HELP_ABOUT = "about.png",
            ICON_SETTINGS = "settings.png",
            ICON_INTERPOL = "interpolate.png";

    private static final String
            SACT_EXIT = "Выход",
            SACT_OPENDOC = "Открыть настройки",
            SACT_SAVEDOC = "Сохранить настройки",
            SACT_COLORS = "Цвета / Градации серого",
            SACT_INTERACT = "Интерактивное построение уровня вкл/выкл",
            SACT_GRID = "Отображение сетки вкл/выкл",
            SACT_ISOLINES = "Отображение линий уровня вкл/выкл",
            SACT_SETTINGS = "Настройка сетки и dom f",
            SACT_INTERPOL = "Интерполяция",
            SACT_ABOUT = "Об авторе";

    private static final String
            MENU_FILE = "Файл",
            MENU_HELP = "Справка";

    private static final String aboutString =
            "Отрисовка линий уровня\n" +
            "Василий Литвинов <kvas.omsk at gmail.com>";

    private Controller ctl;
    private JNiceView fnView, legView;
    private JLabel statusLabel;

    public MainWindow(Controller ctl, ViewSettings vs) {
        this.ctl = ctl;
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle(appTitle);
        setIconImage(new ImageIcon(ICON_SH_ISOLINES).getImage());
        Dimension minSz = new Dimension(800,600);
        setSize(minSz);
        setMinimumSize(minSz);
        addMenu();
        addToolbar();

        JPanel panel = new JPanel(new BorderLayout());

        fnView = new JNiceView(500, 500, vs);
        legView = new JNiceView(40, 500, vs);
        legView.setDrawXLabels(false);
        legView.setDrawXGrid(false);
        legView.setDrawYGrid(true);
        legView.setDrawIsolines(false);
        legView.setTickMod(1);

        panel.add(fnView, BorderLayout.WEST);
        panel.add(legView, BorderLayout.EAST);

        ComponentAdapter ca = new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                if (e != null)
                    super.componentResized(e);
                Dimension newSz = panel.getSize();
                panel.removeAll();
                fnView.changeSize((newSz.width * 9) / 10, newSz.height - 10);
                legView.changeSize(newSz.width - (newSz.width * 9) / 10 - 10, newSz.height - 10);
                panel.add(fnView, BorderLayout.WEST);
                panel.add(legView, BorderLayout.EAST);
            }
        };

        StringBuilder statusBld = new StringBuilder();
        fnView.addMouseMotionListener(new MouseAdapter() {
            @Override
            public void mouseMoved(MouseEvent e) {
                super.mouseMoved(e);
                statusBld.setLength(0);
                statusBld.append('(').append(fnView.getRealX(e.getX())).append(',').append(fnView.getRealY(e.getY())).append(')');
                SwingUtilities.invokeLater(() -> statusLabel.setText(statusBld.toString()));
            }
        });

        fnView.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (interact) {
                    double rx = fnView.getRealX(e.getX());
                    double ry = fnView.getRealY(e.getY());
                    fnView.setUserIsoline(rx, ry);
                }
            }
        });

        addComponentListener(ca);

        add(panel);

        JPanel statusPanel = new JPanel();
        statusPanel.add(statusLabel = new JLabel(":-)"));
        add(statusPanel, BorderLayout.SOUTH);

        pack();
        ca.componentResized(null);

        setLocationRelativeTo(null);
        setVisible(true);
    }

    private ActionListener killListener = (e) -> { System.exit(0); };

    JFileChooser fchooser = new JFileChooser();
    private ActionListener openDocListener = (e) -> {
        SwingUtilities.invokeLater(() -> {
            int rv = fchooser.showOpenDialog(MainWindow.this);
            if (rv == JFileChooser.APPROVE_OPTION) {
                File fl = fchooser.getSelectedFile();
                try {
                    fnView.unsetUserIsoline();
                    ctl.readSettings(fl.getCanonicalPath());

                } catch (IOException ex) {
                    JOptionPane.showMessageDialog(MainWindow.this, ex.toString(),
                            "IO exception", JOptionPane.ERROR_MESSAGE);
                }
            }
        });
    };

    private ActionListener saveDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showSaveDialog(MainWindow.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        ctl.writeSettings(fl.getCanonicalPath());
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, "IOException while opening!");
                    }
                }
            });
        }
    };


    private ActionListener aboutListener = (e) -> {
        SwingUtilities.invokeLater(() ->
                JOptionPane.showMessageDialog(MainWindow.this, aboutString));
    };

    private ActionListener settingsListener = (e) -> {
        SwingUtilities.invokeLater(() -> {
            SettingsDialog dlg = new SettingsDialog(MainWindow.this);
            Optional<ModelSettings> dlgRes = dlg.getResult();
            dlgRes.ifPresent((x) -> ctl.newSettings(dlgRes.get()));
        });
    };

    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof GridFunctionModel) {
            fnView.update(o, arg);

            double min = ((GridFunctionModel) arg).getMin();
            double max = ((GridFunctionModel) arg).getMax();
            //System.out.println("min: " + min);
            //System.out.println("max: " + max);
            XYFunction legFn = (x, y) -> (min + (max - min) * y);
            int n = ((GridFunctionModel) arg).getModelSettings().n;
            GridFunctionModel legFnModel = new GridFunctionModel(legFn, new ModelSettings(2, n, n, 0, 1, min, max));
            legFnModel.addObserver(legView);
            legFnModel.newSettings(legFnModel.getModelSettings());
            //legView.update(o, arg);
        }
    }

    private class ItemDesc {
        String title;
        ImageIcon icon;
        ActionListener listener;

        public ItemDesc(String title, String iconPath, ActionListener listener) {
            this.icon = new ImageIcon(iconPath);
            this.listener = listener;
            this.title = title;
        }
    }

    private boolean shgrid = true;
    private boolean interact = false;
    private boolean shcolors = true;
    private boolean shisolines = true;
    private boolean interpol = false;

    private ActionListener interpolListener = (e) -> {
        interpol = !interpol;
        SwingUtilities.invokeLater(() -> fnView.setDrawInterpol(interpol));
    };

    private ActionListener shIsolinesListener = (e) -> {
        shisolines = !shisolines;
        SwingUtilities.invokeLater(() -> fnView.setDrawIsolines(shisolines));
    };

    private ActionListener shColorsListener = (e) -> {
        shcolors = !shcolors;
        SwingUtilities.invokeLater(() -> fnView.setDrawColors(shcolors));
    };

    private ActionListener shGridListener = (e) -> {
        shgrid = !shgrid;
        SwingUtilities.invokeLater(() -> {
            fnView.setDrawGrid(shgrid);
        });
    };

    private ActionListener interactListener = (e) -> {
        interact = !interact;
        if (!interact)
            SwingUtilities.invokeLater(() -> fnView.unsetUserIsoline());
    };

    private final Map<String, ItemDesc> mItems = new HashMap<String,ItemDesc>() {
        {
            put(SACT_EXIT, new ItemDesc(SACT_EXIT, ICON_APP_EXIT, killListener));
            put(SACT_OPENDOC, new ItemDesc(SACT_OPENDOC, ICON_OPEN_DOCUMENT, openDocListener));
            put(SACT_SAVEDOC, new ItemDesc(SACT_SAVEDOC, ICON_SAVE_DOCUMENT, saveDocListener));
            put(SACT_ABOUT, new ItemDesc(SACT_ABOUT, ICON_HELP_ABOUT, aboutListener));
            put(SACT_COLORS, new ItemDesc(SACT_COLORS, ICON_COLORS, shColorsListener));
            put(SACT_GRID, new ItemDesc(SACT_GRID, ICON_GRID, shGridListener));
            put(SACT_ISOLINES, new ItemDesc(SACT_ISOLINES, ICON_SH_ISOLINES, shIsolinesListener));
            put(SACT_INTERACT, new ItemDesc(SACT_INTERACT, ICON_INTERACT, interactListener));
            put(SACT_SETTINGS, new ItemDesc(SACT_SETTINGS, ICON_SETTINGS, settingsListener));
            put(SACT_INTERPOL, new ItemDesc(SACT_INTERPOL, ICON_INTERPOL, interpolListener));
        }
    };

    private void addMenuItem(JMenu menu, String itemName) {
        ItemDesc desc = mItems.get(itemName);
        JMenuItem mitem = new JMenuItem(desc.title);
        mitem.addActionListener(desc.listener);
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
        addMenuItem(fileMenu, SACT_SETTINGS);
        addMenuItem(fileMenu, SACT_COLORS);
        addMenuItem(fileMenu, SACT_GRID);
        addMenuItem(fileMenu, SACT_ISOLINES);
        addMenuItem(fileMenu, SACT_INTERACT);
        addMenuItem(fileMenu, SACT_EXIT);

        return fileMenu;
    }

    private JMenu createHelpMenu() {
        JMenu helpMenu = new JMenu(MENU_HELP);

        addMenuItem(helpMenu, SACT_ABOUT);

        return helpMenu;
    }

    private void addMenu() {
        JMenuBar menu = new JMenuBar();

        menu.add(createFileMenu());
        menu.add(createHelpMenu());

        setJMenuBar(menu);
    }

    private void addToolbar() {
        JToolBar toolbar = new JToolBar();
        toolbar.setFloatable(false);

        addToolbarBtn(toolbar, SACT_SAVEDOC);
        addToolbarBtn(toolbar, SACT_OPENDOC);
        addToolbarBtn(toolbar, SACT_SETTINGS);
        addToolbarBtn(toolbar, SACT_COLORS);
        addToolbarBtn(toolbar, SACT_GRID);
        addToolbarBtn(toolbar, SACT_ISOLINES);
        addToolbarBtn(toolbar, SACT_INTERACT);
        addToolbarBtn(toolbar, SACT_INTERPOL);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_EXIT);

        add(toolbar, BorderLayout.NORTH);
    }
}
