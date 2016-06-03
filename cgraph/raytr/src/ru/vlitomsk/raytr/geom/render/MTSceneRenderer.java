package ru.vlitomsk.raytr.geom.render;

import ru.vlitomsk.raytr.geom.scenic.Scene;

import java.awt.image.BufferedImage;

/**
 * Created by vas on 23.05.2016.
 */
public class MTSceneRenderer extends SceneRenderer {
    private Scene scene;
    private ViewportCameraSettings renderSettings;
    private int nthreads;

    private RenderThread[] renderThreads;

    class RenderThread extends Thread {
        int i;
        BufferedImage img;
        int w, h;
        int raytracingDepth;
        JustRenderer jrend;
        double maxcolval, gamma;
        int quality;

        public RenderThread(JustRenderer jrend, int i, BufferedImage img, int w, int h, int raytracingDepth, double gamma, int quality) {
            this.jrend = jrend;
            this.i = i;
            this.img = img;
            this.w = w;
            this.h = h;
            this.gamma = gamma;
            this.quality = quality;
            this.raytracingDepth = raytracingDepth;
        }

        public int getPixRendered() {
            return jrend.getPixRendered();
        }

        public int getTotPix() {
            return jrend.getTotPix();
        }

        private boolean prerender = true;
        public void setPrerender(boolean prerender) {
            this.prerender = prerender;
        }

        public void setMaxcolval(double maxcolval) {
            this.maxcolval = maxcolval;
        }

        @Override
        public void run() {
            long time = 0;
            if (prerender) {
                time = jrend.prerenderRaytraced(raytracingDepth,  0, w, i * (h/nthreads), (i+1)*(h/nthreads), gamma, quality);
            } else {
                long start = System.nanoTime();
                jrend.postrenderRaytraced(maxcolval, img, 0, w, i * (h/nthreads), (i+1)*(h/nthreads), gamma);
                time = System.nanoTime() - start;
            }
            System.out.println(i + "th time: " + time);
        }
    }

    public MTSceneRenderer(int nthreads, Scene scene, ViewportCameraSettings renderSettings) {
        super(scene, renderSettings);
        this.renderSettings = renderSettings;
        setNthreads(nthreads);
    }

    public void setNthreads(int nthreads) {
        this.nthreads = nthreads;
        int h = (int) renderSettings.getImH();
        if (h % nthreads != 0)
            throw new RuntimeException("h % nthreads != 0");
        renderThreads = new RenderThread[nthreads];
    }

    public long renderRaytraced(BufferedImage img, int raytracingDepth, double gamma, int quality) {
        refillTree();
        long startTime = System.nanoTime();
        int w = (int) renderSettings.getImW();
        int h = (int) renderSettings.getImH();
        for (int i = 0; i < nthreads; ++i) {
            renderThreads[i] = new RenderThread(new JustRenderer(), i, img, w, h, raytracingDepth, gamma, quality);
            renderThreads[i].setPrerender(true);
            renderThreads[i].start();
        }
        double maxval = Double.MIN_VALUE;
        for (int i = 0; i < nthreads; ++i) {
            try {
                renderThreads[i].join();
                maxval = Math.max(maxval, renderThreads[i].jrend.getMaxColorVal());
                System.out.println(renderThreads[i].jrend.getPixRendered());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println("maxval: " + maxval);
        for (int i = 0; i < nthreads; ++i) {
            renderThreads[i] = new RenderThread(renderThreads[i].jrend, i, img, w, h, raytracingDepth, gamma, quality);
            renderThreads[i].setPrerender(false);
            renderThreads[i].setMaxcolval(maxval);
            renderThreads[i].start();
        }
        for (int i = 0; i < nthreads; ++i) {
            try {
                renderThreads[i].join();
                maxval = Math.max(maxval, renderThreads[i].jrend.getMaxColorVal());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        long endTime = System.nanoTime();
        System.out.println("total time: " + (endTime-startTime));
        return endTime - startTime;
    }

    public int getPixRendered() {
        int sum = 0;
        for (int i =0 ; i < nthreads; ++i) {
            sum += renderThreads[i].getPixRendered();
        }
        return sum;
    }

    public int getTotPix() {
        int sum = 0;
        for (int i =0; i < nthreads; ++i) {
            sum += renderThreads[i].getTotPix();
        }
        return sum;
    }
}
