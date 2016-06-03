package ru.nsu.ccfit.litvinov.cg.life.view;

import ru.nsu.ccfit.litvinov.cg.life.ctl.WorkingDocumentController;
import ru.nsu.ccfit.litvinov.cg.life.model.BadFileFormatException;
import ru.nsu.ccfit.litvinov.cg.life.model.Document;
import ru.nsu.ccfit.litvinov.cg.life.model.FieldDisplayProps;
import ru.nsu.ccfit.litvinov.cg.life.model.NewDocParams;

import javax.swing.*;
import javax.swing.border.Border;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by vas on 13.02.16.
 */
public class MainWindow extends JFrame {
    private static final String appTitle = "Conway's vlitomsk's game of life";

    private static final String
            ICON_APP_EXIT = "application-exit.png",
            ICON_NEW_DOCUMENT = "document-new.png",
            ICON_OPEN_DOCUMENT = "document-open.png",
            ICON_SAVE_DOCUMENT = "document-save.png",
            ICON_HELP_ABOUT = "help-about.png",
            ICON_STEP_SINGLE = "go-next.png",
            ICON_STEP_INF = "go-last.png",
            ICON_PARAMS = "params.png",
            ICON_CLEAR = "clear.png",
            ICON_IMPACTS = "impacts.png";

    private static final String
            SACT_EXIT = "Exit",
            SACT_NEWDOC = "New",
            SACT_SAVEDOC = "Save",
            SACT_OPENDOC = "Open",
            SACT_ABOUT = "About",
            SACT_STEP = "Single step",
            SACT_STEPINF = "Infinite steps",
            SACT_PARAMS = "Parameters",
            SACT_CLEAR = "Clear",
            SACT_IMPACTS = "Show impacts";

    private static final String
            MENU_FILE = "File",
            MENU_STEP = "Step",
            MENU_HELP = "Help";

    private static final String aboutString =
            "This is hexagonal Conway's game of life\n" +
            "by Vasiliy Litvinov <kvas.omsk at gmail.com>";

    WorkingDocumentView docView;
    WorkingDocumentController workingDocumentController;

    public MainWindow(WorkingDocumentController workingDocumentController) {
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                killListener.actionPerformed(null);
            }
        });
        setTitle(appTitle);
        Dimension minSz = new Dimension(800,600);
        setSize(minSz);
        setMinimumSize(minSz);

        addMenu();
        addToolbar();
        addDocView(workingDocumentController);

        setLocationRelativeTo(null);
        setVisible(true);
    }

    private void addDocView(WorkingDocumentController workingDocumentController) {
        this.workingDocumentController = workingDocumentController;
        docView = new WorkingDocumentView(workingDocumentController);
        workingDocumentController.subscribe(docView);
        JScrollPane jsp = new JScrollPane();
        jsp.setViewportView(docView);
        add(jsp, BorderLayout.CENTER);
    }

    private ActionListener killListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {

            //System.exit(0);
            stepTimer.stop();
            if (workingDocumentController != null &&
                workingDocumentController.getWdoc() != null &&
                workingDocumentController.getWdoc().getDoc() != null)
            {
                if (workingDocumentController.getWdoc().getDoc().isSaved())
                    dispose();
                else {
                    SwingUtilities.invokeLater(() -> {
                        int dialogButton = JOptionPane.YES_NO_CANCEL_OPTION;
                        int res = JOptionPane.showConfirmDialog (null, "Wanna save dis game, huh?","?",dialogButton);
                        if (res == JOptionPane.YES_OPTION) {
                            saveDocListener.actionPerformed(null);
                            dispose();
                        } else if (res == JOptionPane.NO_OPTION) {
                            dispose();
                        } else {
                            return;
                        }
                    });
                }
            } else {
                dispose();
            }
        }
    };

    private ActionListener newDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                NewDocParams par = new NewDocDialog(MainWindow.this).get();
                if (par != null)
                    workingDocumentController.newDoc(par.m, par.n, par.height, par.thickness);
            });
        }
    };

    private ActionListener paramsListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                NewDocParams par = new ParamDialog(MainWindow.this).get();
                if (par != null) {
                    workingDocumentController.getWdoc().getDoc().getDprops().setCellHeight(par.height);
                    workingDocumentController.getWdoc().getDoc().getDprops().setLineThickness(par.thickness);
                }
            });
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
                        workingDocumentController.openDoc(fl.getCanonicalPath());
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, ex.toString(),
                                "IO exception", JOptionPane.ERROR_MESSAGE);
                    } catch (BadFileFormatException ex) {
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
            //SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showSaveDialog(MainWindow.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        workingDocumentController.saveDoc(fl.getCanonicalPath());
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(MainWindow.this, "IOException while opening!");
                    }
                }
            //});
        }
    };

    private ActionListener aboutListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(MainWindow.this, aboutString));
        }
    };

    private ActionListener stepSingleListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                if (workingDocumentController.getWdoc().getDoc() != null)
                    workingDocumentController.getWdoc().getDoc().getField().nextStep();
            });
        }
    };

    private ActionListener stepInfListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            stepinf_on = !stepinf_on;
            if (stepinf_on) {
                stepTimer.start();
            } else {
                stepTimer.stop();
            }
        }
    };

    private ActionListener clearListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            Document doc = workingDocumentController.getWdoc().getDoc();
            if (doc != null) {
                doc.getField().clear();
            }
        }
    };

    private boolean impshow = false;
    private ActionListener impactsListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            impshow = !impshow;
            docView.showImpacts(impshow);
            docView.forceredraw();
        }
    };

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

    private final Map<String, ItemDesc> mItems = new HashMap<String,ItemDesc>() {
        {
            put(SACT_EXIT, new ItemDesc(SACT_EXIT, ICON_APP_EXIT, killListener));
            put(SACT_NEWDOC, new ItemDesc(SACT_NEWDOC, ICON_NEW_DOCUMENT, newDocListener));
            put(SACT_OPENDOC, new ItemDesc(SACT_OPENDOC, ICON_OPEN_DOCUMENT, openDocListener));
            put(SACT_SAVEDOC, new ItemDesc(SACT_SAVEDOC, ICON_SAVE_DOCUMENT, saveDocListener));
            put(SACT_ABOUT, new ItemDesc(SACT_ABOUT, ICON_HELP_ABOUT, aboutListener));
            put(SACT_STEP, new ItemDesc(SACT_STEP, ICON_STEP_SINGLE, stepSingleListener));
            put(SACT_STEPINF, new ItemDesc(SACT_STEPINF, ICON_STEP_INF, stepInfListener));
            put(SACT_PARAMS, new ItemDesc(SACT_PARAMS, ICON_PARAMS, paramsListener));
            put(SACT_CLEAR, new ItemDesc(SACT_CLEAR, ICON_CLEAR, clearListener));
            put(SACT_IMPACTS, new ItemDesc(SACT_IMPACTS, ICON_IMPACTS, impactsListener));
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

    boolean stepinf_on = false;
    private Timer stepTimer = new Timer(1000, stepSingleListener);
    {
        stepTimer.setInitialDelay(0);
        stepTimer.setRepeats(true);
    }

    private void addToolbarToggleBtn(JToolBar tb, String itemName) {
        ItemDesc desc = mItems.get(itemName);
        JToggleButton btn = new JToggleButton();
        btn.addActionListener(desc.listener);
        btn.setIcon(desc.icon);
        btn.setToolTipText(desc.title);
        tb.add(btn);
    }

    private JMenu createFileMenu() {
        JMenu fileMenu = new JMenu(MENU_FILE);

        addMenuItem(fileMenu, SACT_NEWDOC);
        addMenuItem(fileMenu, SACT_SAVEDOC);
        addMenuItem(fileMenu, SACT_OPENDOC);
        fileMenu.addSeparator();
        addMenuItem(fileMenu, SACT_PARAMS);
        addMenuItem(fileMenu, SACT_IMPACTS);
        addMenuItem(fileMenu, SACT_CLEAR);
        fileMenu.addSeparator();
        addMenuItem(fileMenu, SACT_EXIT);

        return fileMenu;
    }

    private JMenu createHelpMenu() {
        JMenu helpMenu = new JMenu(MENU_HELP);

        addMenuItem(helpMenu, SACT_ABOUT);

        return helpMenu;
    }

    private JMenu createStepMenu() {
        JMenu stepMenu = new JMenu(MENU_STEP);
        addMenuItem(stepMenu, SACT_STEP);
        addMenuItem(stepMenu, SACT_STEPINF);
        return stepMenu;
    }

    private void addMenu() {
        JMenuBar menu = new JMenuBar();

        menu.add(createFileMenu());
        menu.add(createStepMenu());
        menu.add(createHelpMenu());

        setJMenuBar(menu);
    }

    private void addToolbar() {
        JToolBar toolbar = new JToolBar();
        toolbar.setFloatable(false);

        addToolbarBtn(toolbar, SACT_NEWDOC);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_SAVEDOC);
        addToolbarBtn(toolbar, SACT_OPENDOC);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_STEP);
        addToolbarToggleBtn(toolbar, SACT_STEPINF);
        addToolbarBtn(toolbar, SACT_CLEAR);

        toolbar.addSeparator();

        addToolbarToggleBtn(toolbar, SACT_IMPACTS);
        addToolbarBtn(toolbar, SACT_PARAMS);

        toolbar.addSeparator();

        add(toolbar, BorderLayout.NORTH);
        JRadioButton xorBtn = new JRadioButton("XOR");
        xorBtn.setSelected(true);
        xorBtn.addActionListener((e) -> {
            if (workingDocumentController.getFieldCtl() != null) {
                workingDocumentController.getFieldCtl().setXorMode();
            }
        });
        JRadioButton setBtn = new JRadioButton("SET");
        setBtn.addActionListener((e) -> {
            if (workingDocumentController.getFieldCtl() != null) {
                workingDocumentController.getFieldCtl().setSetMode();
            }
        });
        ButtonGroup grp = new ButtonGroup();
        grp.add(xorBtn);
        grp.add(setBtn);
        toolbar.add(xorBtn);
        toolbar.add(setBtn);

        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_EXIT);

    }
}
