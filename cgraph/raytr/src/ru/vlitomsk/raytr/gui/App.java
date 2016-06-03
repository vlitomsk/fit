package ru.vlitomsk.raytr.gui;

import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.render.CameraReader;
//import ru.vlitomsk.raytr.geom.render.MTMTSceneRenderer;
import ru.vlitomsk.raytr.geom.render.ViewportCameraSettings;
import ru.vlitomsk.raytr.geom.render.MTSceneRenderer;
import ru.vlitomsk.raytr.geom.samples.SnowmanScene;
import ru.vlitomsk.raytr.geom.scenic.Scene;
import ru.vlitomsk.raytr.geom.scenic.SceneReader;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by vas on 13.02.16.
 */
public class App extends JFrame {
    private static final String appTitle = "vlitomsk's raytracing lab";

    private static final String
            ICON_APP_EXIT = "application-exit.png",
            ICON_OPEN_DOCUMENT = "document-open.png",
            ICON_OPEN_CAM = "cam-open.png",
            ICON_SAVE_DOCUMENT = "document-save.png",
            ICON_SETTINGS = "settings.png",
            ICON_HELP_ABOUT = "help-about.png",
            ICON_MODE_WF = "mode-wireframe.png",
            ICON_MODE_RT = "mode-raytrace.png";

    private static final String
            SACT_EXIT = "Exit",
            SACT_SAVEDOC = "Save BMP",
            SACT_OPENDOC = "Open model",
            SACT_OPENCAM = "Open cam file",
            SACT_SETTINGS = "Settings",
            SACT_ABOUT = "About",
            SACT_MODE_WF = "Wireframe mode",
            SACT_MODE_RT = "Raytrace mode";

    private static final String
            MENU_FILE = "File",
            MENU_HELP = "Help";

    private static final String aboutString =
            "This is raytracing lab\n" +
                    "by Vasiliy Litvinov <kvas.omsk at gmail.com>";

    Scene scene = new SnowmanScene();
//    Scene scene;
//    {
//        try {
//            scene = SceneReader.readScene(new FileInputStream("h:\\bunny.txt"));
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//    }
    ViewportCameraSettings rsett = new ViewportCameraSettings();
    {
    rsett.setBgColor(Color.black);
    rsett.setLineColor(Color.black);
    }
    private static final int NTHREADS = 1;
    MTSceneRenderer renderer = new MTSceneRenderer(NTHREADS, scene, rsett);

    private static boolean RENDER_WIREFRAME = false, RENDER_RAYS = true;
    private boolean renderMode = RENDER_WIREFRAME;
    private JLabel imgLabel;
    private BufferedImage bufImg = new BufferedImage(800, 600, BufferedImage.TYPE_3BYTE_BGR);
    private SettingsResult defaultSettings = new SettingsResult(Color.black, 1.0, 2, 2);

    private void doRender() {
        new Thread(() -> {
            for (Component comp : toolbar.getComponents()) {
                comp.setEnabled(false);
            }
            synchronized (renderer) {
                if (renderMode == RENDER_WIREFRAME) {
                    renderer.renderWireframe(bufImg);
                } else {
                    renderer.renderRaytraced(bufImg, defaultSettings.depth, defaultSettings.gamma, defaultSettings.quality);
                }
            }
            ImageIcon imicon = new ImageIcon(bufImg);
            imgLabel.setIcon(imicon);
            for (Component comp : toolbar.getComponents()) {
                comp.setEnabled(true);
            }
        }).start();
    }

    private Vec camDir = new Vec();
    private Point mousePt = new Point(100, 100);
    private Vec deltaVec = new Vec();
    public App() {
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle(appTitle);
        ImageIcon appicon = new ImageIcon(ICON_MODE_RT);
        setIconImage(appicon.getImage());
        Dimension minSz = new Dimension(800,600);
        setSize(minSz);
        setMinimumSize(minSz);

        addMenu();
        addToolbar();

        imgLabel = new JLabel("");
        ImageIcon imicon = new ImageIcon(bufImg);
        imgLabel.setIcon(imicon);
        add(imgLabel);

        SwingUtilities.invokeLater(() -> doRender());

        imgLabel.addMouseWheelListener(e1 -> {
            int rot = e1.getWheelRotation();

            if (!e1.isControlDown()) {
                final double factor = 1.1;
                double coef = rot < 0 ? factor : 1.0 / factor;
                rsett.setZf(rsett.getZf() * coef);
            } else {
                rsett.getCamPos().padd(camDir.assign3(rsett.getCamView()).padd(rsett.getCamPos(), -1).pnormalize3d().pcoef3(rot>0?1:-1));
            }
            synchronized (renderer) {
                renderer.applyViewportCameraSettings(rsett);
            }
            doRender();
        });

        imgLabel.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                mousePt = e.getPoint();
            }
        });

        imgLabel.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                //super.mouseDragged(e);

                int dx = e.getX() - mousePt.x;
                int dy = e.getY() - mousePt.y;
                double dst = deltaVec.assign3(rsett.getCamPos()).padd(rsett.getCamView(), -1).norm3d();
                double phi = dx * 0.05 / 180.0 * Math.PI;
                deltaVec.pcoef3(Math.cos(phi)/dst).padd(rsett.getI(), Math.sin(phi)).pcoef3(dst).padd(rsett.getCamView());
                rsett.setCamPos(rsett.getCamPos().assign3(deltaVec));

                phi = -dy * 0.05 / 180.0 * Math.PI;
                deltaVec.assign3(rsett.getK()).pcoef3(Math.cos(phi)).padd(rsett.getJ(), Math.sin(phi)).pcoef3(dst).padd(rsett.getCamView());
                rsett.setCamPos(rsett.getCamPos().assign3(deltaVec));

                synchronized (renderer) {
                    renderer.applyViewportCameraSettings(rsett);
                }
                doRender();
                mousePt.x += dx;
                mousePt.y += dy;
            }
        });

        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                double dx=0, dy=0;
                switch (e.getKeyCode()) {
                    case KeyEvent.VK_A:
                        dx = -1;
                        break;
                    case KeyEvent.VK_D:
                        dx = 1;
                        break;
                    case KeyEvent.VK_W:
                        dy = 1;
                        break;
                    case KeyEvent.VK_S:
                        dy = -1;
                        break;
                }

                dx *= 0.3;
                dy *= 0.3;
                if (dx != 0 || dy != 0) {
                    //System.out.println(dx + " " + dy);
                    rsett.getCamPos().padd(rsett.getI(), dx).padd(rsett.getJ(), dy);
                    rsett.getCamView().padd(rsett.getI(), dx).padd(rsett.getJ(), dy);
                    rsett.setCamPos(rsett.getCamPos());
                    synchronized (renderer) {
                        renderer.applyViewportCameraSettings(rsett);
                    }
                    doRender();
                }
            }
        });

        setLocationRelativeTo(null);
        setVisible(true);

        new Thread(() -> {
            while (true) {
                progressBar.setValue(renderer.getProgress());
                try {
                    Thread.sleep(300);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private ActionListener killListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            System.exit(0);
        }
    };

    private ActionListener wfModeListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                renderMode = RENDER_WIREFRAME;
                doRender();
            });
        }
    };

    private ActionListener rtModeListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                renderMode = RENDER_RAYS;
                doRender();
            });
        }
    };

    JFileChooser fchooser = new JFileChooser();
    private ActionListener openDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showOpenDialog(App.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        scene = SceneReader.readScene(new FileInputStream(fl));
                        renderer = new MTSceneRenderer(NTHREADS, scene, rsett);
                        doRender();
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });
        }

    };

    private ActionListener openCamListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showOpenDialog(App.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    ViewportCameraSettings vcs;
                    try {
                        vcs=CameraReader.readSettings(new FileInputStream(fl));
                        synchronized (renderer) {
                            renderer.applyViewportCameraSettings(vcs);
                        }
                        rsett = vcs;
                        doRender();
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });
        }

    };

    private ActionListener saveDocListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() -> {
                int rv = fchooser.showSaveDialog(App.this);
                if (rv == JFileChooser.APPROVE_OPTION) {
                    File fl = fchooser.getSelectedFile();
                    try {
                        ImageIO.write(bufImg, "BMP", fl);
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });
        }
    };

    private ActionListener aboutListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(App.this, aboutString));
        }
    };

    private int depth;
    private int quality;
    private double gamma;

    private ActionListener settingsListener = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
            SettingsDialog dial = new SettingsDialog(App.this, defaultSettings);
            SettingsResult res = dial.getResult();
            if (res == null)
                return;
            rsett.setBgColor(res.bgColor);
            depth = res.depth;
            quality = res.quality;
            defaultSettings = res;
            doRender();
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
            //put(SACT_NEWDOC, new ItemDesc(SACT_NEWDOC, ICON_NEW_DOCUMENT, newDocListener));
            put(SACT_OPENDOC, new ItemDesc(SACT_OPENDOC, ICON_OPEN_DOCUMENT, openDocListener));
            put(SACT_OPENCAM, new ItemDesc(SACT_OPENCAM, ICON_OPEN_CAM, openCamListener));
            put(SACT_SAVEDOC, new ItemDesc(SACT_SAVEDOC, ICON_SAVE_DOCUMENT, saveDocListener));
            put(SACT_SETTINGS, new ItemDesc(SACT_SETTINGS, ICON_SETTINGS, settingsListener));
            put(SACT_ABOUT, new ItemDesc(SACT_ABOUT, ICON_HELP_ABOUT, aboutListener));
            put(SACT_MODE_RT, new ItemDesc(SACT_MODE_RT, ICON_MODE_RT, rtModeListener));
            put(SACT_MODE_WF, new ItemDesc(SACT_MODE_WF, ICON_MODE_WF, wfModeListener));
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
        btn.setFocusable(false);
        btn.setIcon(desc.icon);
        btn.addActionListener(desc.listener);
        btn.setToolTipText(desc.title);
        tb.add(btn);
    }

    private JMenu createFileMenu() {
        JMenu fileMenu = new JMenu(MENU_FILE);

        //addMenuItem(fileMenu, SACT_NEWDOC);
        addMenuItem(fileMenu, SACT_SAVEDOC);
        addMenuItem(fileMenu, SACT_OPENDOC);
        addMenuItem(fileMenu, SACT_OPENCAM);
        addMenuItem(fileMenu, SACT_SETTINGS);
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

    private JProgressBar progressBar;

    private JToolBar toolbar;
    private void addToolbar() {
        toolbar = new JToolBar();
        toolbar.setFloatable(false);

        //addToolbarBtn(toolbar, SACT_NEWDOC);
        //toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_SAVEDOC);
        addToolbarBtn(toolbar, SACT_OPENDOC);
        addToolbarBtn(toolbar, SACT_OPENCAM);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_MODE_WF);
        addToolbarBtn(toolbar, SACT_MODE_RT);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_SETTINGS);
        toolbar.addSeparator();
        addToolbarBtn(toolbar, SACT_EXIT);
        toolbar.addSeparator();
        progressBar =  new JProgressBar(0, 100);
        progressBar.setValue(40);
        progressBar.setStringPainted(true);
        progressBar.setPreferredSize(new Dimension(200, 30));
        toolbar.add(progressBar);

        add(toolbar, BorderLayout.NORTH);
    }
}
