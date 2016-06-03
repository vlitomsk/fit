package ru.vlitomsk.raytr.gui;

//import ru.vlitomsk.raytr.geom.render.MTSceneRenderer;

import javax.swing.*;

public class Raytrace extends JFrame {
//    private double phi = 0;
//    private double radius = 10;
//
//    public Raytrace() {
//        setTitle("vlitomsk's raytracing lab");
//        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
//        Dimension windowSize = new Dimension(800, 600);
//        setSize(windowSize);
//        //setMinimumSize(windowSize);
//        setResizable(false);
//
//        BufferedImage bufImg = new BufferedImage(800, 600, BufferedImage.TYPE_3BYTE_BGR);
//        //new SnowmanScene().renderRaytraced(bufImg, 1);
//        Scene scene = new SnowmanScene();
//        ViewportCameraSettings rsett = new ViewportCameraSettings();
//        rsett.setBgColor(Color.darkGray);
//        rsett.setLineColor(Color.black);
//
//        //SceneRenderer renderer = new SceneRenderer(scene, rsett);
//        //MTSceneRenderer renderer = new MTSceneRenderer(2, scene, rsett);
//        SceneRenderer renderer = new SceneRenderer(scene, rsett);
//        //renderer.renderRaytraced(bufImg, 3);
//        renderer.renderWireframe(bufImg);
//
//        JLabel label = new JLabel("");
//        ImageIcon imicon = new ImageIcon(bufImg);
//        label.setIcon(imicon);
//        add(label);
//
//        setVisible(true);
//
////        new Thread(() -> {
////            while (true) {
////                phi += Math.PI / 180;
////                rsett.setCamPos(new Vec(radius * Math.cos(phi), radius * Math.sin(phi), 10));
////                renderer.applyRenderSettings(rsett);
////                long tm = renderer.renderRaytraced(bufImg, 1);
////                System.out.println("tm: " + (double) tm * 1e-6);
////                label.setIcon(new ImageIcon(bufImg));
////                try {
////                    Thread.sleep(30);
////                } catch (InterruptedException e) {
////                    e.printStackTrace();
////                }
////            }
////        }).start();
//
////        label.addMouseListener(new MouseAdapter() {
////            @Override
////            public void mousePressed(MouseEvent e) {
////                SwingUtilities.invokeLater(() -> {
////                    synchronized (renderer) {
////                        phi += Math.PI / 6;
////                        rsett.setCamPos(new Vec(radius * Math.cos(phi), radius * Math.sin(phi), 10));
////                        renderer.applyRenderSettings(rsett);
////                        long tm = renderer.renderRaytraced(bufImg, 1);
////                        System.out.println("tm: " + (double)tm * 1e-6);
////                        label.setIcon(new ImageIcon(bufImg));
////                    }
////                });
////            }
////        });
//    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new App());
    }
}
