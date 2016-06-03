package ru.vlitomsk.raytr.geom.render;

import ru.vlitomsk.raytr.geom.base.Inters;
import ru.vlitomsk.raytr.geom.base.Segment;
import ru.vlitomsk.raytr.geom.base.Vec;
import ru.vlitomsk.raytr.geom.scenic.Light;
import ru.vlitomsk.raytr.geom.scenic.OpticParams;
import ru.vlitomsk.raytr.geom.scenic.Scene;
import ru.vlitomsk.raytr.geom.scenic.SceneObject;
import ru.vlitomsk.raytr.geom.transform.Linop;
import ru.vlitomsk.raytr.geom.transform.PerspDivisionTransform;
import ru.vlitomsk.raytr.geom.transform.SingleLinopTransform;
import ru.vlitomsk.raytr.geom.transform.Transform;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

import static java.lang.Math.*;

/**
 * Created by vas on 23.05.2016.
 */
public class OldSceneRenderer {
    private Scene scene;

    private ViewportCameraSettings camSettings;

    private Linop linop = new Linop();

    public void applyViewportCameraSettings(ViewportCameraSettings renderSettings) {
        this.camSettings = renderSettings;
        camtrans = new SingleLinopTransform(linop.camTransMat(renderSettings.getCamPos()));
        camrot = new SingleLinopTransform(linop.camRotMat(renderSettings.getCamPos(), renderSettings.getCamView(), renderSettings.getCamUp()));
        proj = new SingleLinopTransform(linop.perspProj(renderSettings.getSw(), renderSettings.getSh(), renderSettings.getZf(), renderSettings.getZb()));
        pdiv = new PerspDivisionTransform();

        objectTransformChain = new ArrayList<Transform>() {{
            add(camtrans);
            add(camrot);
        }};

        segmentTransformChain = new ArrayList<Transform>() {{
            add(proj);
            add(pdiv);
            add(new Transform() {
                @Override
                public Vec ptransform(Vec v) {
                    return v.pcoef3(renderSettings.getImW() / renderSettings.getSw()).padd(renderSettings.getCenter());
                }
            });
        }};

        reassignSceneObjects();
        doTransforms();
    }

    private SingleLinopTransform camtrans = new SingleLinopTransform(), camrot = new SingleLinopTransform(), proj = new SingleLinopTransform();
    private PerspDivisionTransform pdiv;

    List<Transform> objectTransformChain;
    List<Transform> segmentTransformChain;

    private SceneObject[] origSceneObjects, sceneObjects;
    private Light[] origLights, clonedLights;
    List<Segment> segmentsTransformed = new ArrayList<Segment>();
    public OldSceneRenderer(Scene scene, ViewportCameraSettings camSettings) {
        this.scene = scene;
        origSceneObjects = scene.getSceneObjects();
        origLights = scene.getLights();
        sceneObjects = new SceneObject[origSceneObjects.length];
        clonedLights = new Light[origLights.length];
        for (int i =0; i < sceneObjects.length; ++i) {
            sceneObjects[i] = origSceneObjects[i].clone();
        }
        for (int i =0 ; i < clonedLights.length; ++i) {
            clonedLights[i] = origLights[i].clone();
        }
        applyViewportCameraSettings(camSettings);
        //reassignSceneObjects();
        //doTransforms();
    }

    /**
     * эта функция работает с предположением, что набор объектов неизменный
     * и хранится в неизменном порядке
     */
    private void reassignSceneObjects() {
        origSceneObjects = scene.getSceneObjects();
        origLights = scene.getLights();
        for (int i = 0; i < origSceneObjects.length; ++i) {
            sceneObjects[i].assign(origSceneObjects[i]);
        }

        for (int i = 0; i < origLights.length; ++i) {
            clonedLights[i].assign(origLights[i]);
        }
    }

    /**
     * на входе -- присвоенные копии объектов в sceneObjects[]
     * на выходе -- те же самые копии, оттрансформированные А ТАКЖЕ segmentsTransformed[]
     */
    private void doTransforms() {
        transformObjects(objectTransformChain);
        transformLights(objectTransformChain);
        transformSegments(segmentTransformChain);
    }

    private void transformObjects(List<Transform> objectTransformChain) {
        for (SceneObject sobj : sceneObjects) {
            for (Transform tr : objectTransformChain) {
                sobj.ptransform(tr);
            }
        }
    }

    private void transformLights(List<Transform> lightTransformChain) {
        for (Light l : clonedLights) {
            for (Transform tr : lightTransformChain) {
                l.ptransform(tr);
            }
        }
    }

    private void transformSegments(List<Transform> segmentTransformChain) {
        segmentsTransformed.clear();
        for (SceneObject sobj : sceneObjects) {
            Segment[] segments = sobj.getSegments();
            for (Segment seg : segments) {
                Vec[] pts = new Vec[] {seg.p[0].clone(), seg.p[1].clone()};
                for (Vec v : pts) {
                    for (Transform trans : segmentTransformChain) {
                        trans.ptransform(v);
                    }
                }
                segmentsTransformed.add(new Segment(pts));
            }
        }
    }

    private int rn(double x) {
        return (int) round(x);
    }

    class JustRenderer {
        private Vec isectDistVec = new Vec(0,0);
        private Vec lgtDir = new Vec();
        private Vec hvec = new Vec();
        private Inters inters = new Inters();

        Vec minIsect = new Vec();
        Vec surfNorm = new Vec();
        Vec diffLightColor = new Vec();
        Vec objColor = new Vec();
        Vec coefMirror = new Vec();
        Vec ltColor = new Vec();
        Vec lineColorIntens = new Vec();
        Vec bgColorIntens = new Vec();
        Vec zeroIntens = new Vec(0,0,0,0);
        static final int MAX_DEPTH = 5;
        Vec intens[] = new Vec[MAX_DEPTH];
        Vec refl[] = new Vec[MAX_DEPTH];

        {
            for (int i = 0; i < MAX_DEPTH; ++i) {
                intens[i] = new Vec();
                refl[i] = new Vec();
            }
        }

        private Vec traceRay(Vec raypt, Vec normalizedDir, int depth) {
            //Vec minIsect = null;
            boolean anyIsection = false;
            SceneObject minIsectObj = null;
            double minSqrNorm = Double.MAX_VALUE;

            for (SceneObject obj : sceneObjects) {
                Vec isect = obj.rayIntersect(inters, raypt, normalizedDir);
                if (isect == null)
                    continue;
                anyIsection = true;
                double sqrNorm = isectDistVec.assign3(isect).padd(raypt, -1.0).sqrNorm3d();
                if (sqrNorm < minSqrNorm) {
                    minIsect.assign3(isect);
                    minSqrNorm = sqrNorm;
                    minIsectObj = obj;
                }
            }

            if (!anyIsection)
                return null;

            OpticParams opa = minIsectObj.getOpticParams();
            minIsectObj.calcNorm(minIsect, surfNorm);

            intens[depth] = color2vec(intens[depth], scene.getDiffuseLightColor()).
                    //pcoef3(1.0/ 255.0).
                    pcompwiseMul3(opa.coefDiffuse);
                    //pcompwiseMul3(color2vec(objColor, opa.color));

            for (Light lgt : clonedLights) {
                double lgtDist = openSpaceDist(lgt.pos, minIsect);

                if (lgtDist < 0) {
                    continue;
                }

                double fatt = 100 / (1.0 + lgtDist);
                lgtDir.assign3(lgt.pos).padd(minIsect, -1).pnormalize3d();
                Vec lgtSummand = objColor.assign3(opa.coefDiffuse).pcoef3(abs(surfNorm.dot(3,lgtDir)));
                hvec.assign3(normalizedDir).pcoef3(-1).padd(lgtDir).pnormalize3d();
                lgtSummand.padd(coefMirror.assign3(opa.coefMirror).pcoef3(Math.pow((surfNorm.dot(3,hvec)), opa.power)));

                lgtSummand.pcoef3(fatt);
                lgtSummand.pcompwiseMul3(color2vec(ltColor, lgt.color));

                //lgtSummand.pcoef3(1.0/255.0);
                intens[depth].padd(lgtSummand);
            }
            //intens[depth].pcoef3(0.01);
            if (depth >= 2) {
                Vec Evec = normalizedDir.clone().pcoef3(-1);
                Vec vmid = surfNorm.clone().pcoef3(surfNorm.dot(3, Evec));
                refl[depth].assign3(vmid).padd(vmid.clone().padd(Evec,-1));
                //refl[depth].assign(surfNorm).pcoef3(-2.0 * surfNorm.dot(3, Evec)).padd(Evec);
                Vec intensR = traceRay(minIsect.padd(normalizedDir, -0.001).clone(), refl[depth], depth - 1);
                if (intensR != null)
                    intens[depth].padd(coefMirror.assign3(opa.coefMirror).pcompwiseMul3(intensR));
            }

            //intens[depth].psatur3(0, 255);
            //return rgbcol(intens);
            return intens[depth];
        }

        private Vec ospaceDir = new Vec();
        private Vec tmpv = new Vec();
        /**
         * @return <= 0, если нет прямого пути
         */
        private double openSpaceDist(Vec a, Vec b) {
            double dst = ospaceDir.assign3(b).padd(a, -1).norm3d();
            ospaceDir.pcoef3(1/dst);

            for (SceneObject obj : sceneObjects) {
                Vec isect = obj.rayIntersect(inters, a, ospaceDir);
                if (isect != null) {
                    double dst1 = tmpv.assign3(isect).padd(a, -1).norm3d();
                    if (dst1 < dst - 1e-6)
                        return -1;
                }
            }
            return dst;
        }

        private Vec color2vec(Vec v, Color col) {
            v.comp[0] = col.getRed();
            v.comp[1] = col.getGreen();
            v.comp[2] = col.getBlue();
            v.comp[3] = 0;
            return v;
        }

        private int rgbcol(Vec v) {
            return rn(v.z()) | rn(v.y()) << 8 | rn(v.x()) << 16;
        }

        private int pixRendered = 0;
        private int totPix = 0;

        public int getPixRendered() {
            return pixRendered;
        }

        public int getTotPix() {
            return totPix;
        }

        private double maxColorVal;
        private Vec[][] prerenderMat;

        public long prerenderRaytraced(int raytracingDepth, int i0, int i1, int j0, int j1, double gamma, int quality) {
            prerenderMat = new Vec[i1-i0][j1-j0];
            maxColorVal = Double.MIN_VALUE;
            long startTime = System.nanoTime();
            color2vec(lineColorIntens, camSettings.getLineColor());
            color2vec(bgColorIntens, camSettings.getBgColor());
            final int w = (int) camSettings.getImW();
            final int h = (int) camSettings.getImH();
            Vec dir = new Vec(0,0);
            Vec raypt = new Vec(0,0,0,1);
            totPix = (i1-i0)*(j1-j0);
            pixRendered = 0;
            if (quality == 0) { // poor
                for (int j = j0; j < j1; j+=2) {
                    for (int i = i0; i < i1; i+=2) {
                        dir.comp[0] = camSettings.getXmult()*(i - camSettings.getImW() * 0.5);
                        dir.comp[1] = camSettings.getYmult()*((h-j) - camSettings.getImH() * 0.5);
                        dir.comp[2] = camSettings.getZf();
                        dir.comp[3] = 0;

                        dir.pnormalize3d();

                        Vec inte = traceRay(raypt, dir, raytracingDepth);
                        if (inte != null) {
                            for (int u = -1; u <= 0; ++u) {
                                for (int v = -1; v <= 0; ++v) {
                                    if (i-i0+u >=0 && j-j0+v >= 0)
                                    prerenderMat[i - i0 + u][j - j0 + v] = inte.clone();
                                }
                            }

                            maxColorVal = max(maxColorVal, max(inte.x(), max(inte.y(), inte.z())));
                        }
                        pixRendered += 4;
                    }
                }
            } else if (quality == 1) { // normal
                for (int j = j0; j < j1; ++j) {
                    for (int i = i0; i < i1; ++i) {
                        dir.comp[0] = camSettings.getXmult()*(i+0.5 - camSettings.getImW() * 0.5);
                        dir.comp[1] = camSettings.getYmult()*((h-j-0.5) - camSettings.getImH() * 0.5);
                        dir.comp[2] = camSettings.getZf();
                        dir.comp[3] = 0;

                        dir.pnormalize3d();

                        Vec inte = traceRay(raypt, dir, raytracingDepth);
                        if (inte != null) {
                            prerenderMat[i - i0][j - j0] = inte.clone();
                            maxColorVal = max(maxColorVal, max(inte.x(), max(inte.y(), inte.z())));
                        }
                        ++pixRendered;
                    }
                }
            } else if (quality == 2) { // fine
                Vec sum = new Vec();
                for (int j = j0; j < j1; ++j) {
                    for (int i = i0; i < i1; ++i) {
                        sum.pcoef4(0);
                        for (double dj = 0.25; dj < 0.751; dj += 0.5) {
                            for (double di = 0.25; di < 0.751; di += 0.5) {
                                dir.comp[0] = camSettings.getXmult()*(i+di - camSettings.getImW() * 0.5);
                                dir.comp[1] = camSettings.getYmult()*((h-j-dj) - camSettings.getImH() * 0.5);
                                dir.comp[2] = camSettings.getZf();
                                dir.comp[3] = 0;

                                dir.pnormalize3d();

                                Vec inte = traceRay(raypt, dir, raytracingDepth);
                                if (inte != null)
                                    sum.padd(inte, 0.25);
                            }
                        }

                        prerenderMat[i - i0][j - j0] = sum.clone();
                        maxColorVal = max(maxColorVal, max(sum.x(), max(sum.y(), sum.z())));

                        ++pixRendered;
                    }
                }
            } else
                throw new RuntimeException("This quality is not supported: " + quality);

            long endTime = System.nanoTime();
            return endTime - startTime;
        }

        public double getMaxColorVal() {
            return maxColorVal;
        }

        public void postrenderRaytraced(double maxColorVal, BufferedImage img, int i0, int i1, int j0, int j1, double gamma) {
            int bgRgb = camSettings.getBgColor().getRGB();
            double coef = 255.0/Math.pow(maxColorVal, gamma);
            for (int j = j0; j < j1; ++j) {
                for (int i = i0; i < i1; ++i) {
                    Vec v = prerenderMat[i-i0][j-j0];
                    if (v != null) {
                        img.setRGB(i, j, rgbcol(prerenderMat[i - i0][j - j0].pcoef3(coef).ppow3(gamma).psatur3(0,255)));
                    } else {
                        img.setRGB(i, j, bgRgb);
                    }
                }
            }
        }

        public void renderWireframe(BufferedImage img) {
            color2vec(lineColorIntens, camSettings.getLineColor());
            //color2vec(bgColorIntens, camSettings.getBgColor());
            Graphics2D g2 = img.createGraphics();
            g2.setBackground(camSettings.getBgColor());
            g2.clearRect(0, 0, img.getWidth(), img.getHeight());
            drawSegments(g2);
        }

        private void drawSegments(Graphics2D g2) {
            //g2.setColor(lineColor);
            g2.setColor(Color.red);

            for (Segment st : segmentsTransformed) {
                g2.drawLine(rn(st.p[0].x()), rn(camSettings.getImH() - st.p[0].y()), rn(st.p[1].x()), rn(camSettings.getImH() - st.p[1].y()));
            }
        }
    }

    public int getProgress() {
        if (justRenderer.getTotPix() == 0)
            return 0;
        return (100*justRenderer.getPixRendered()) / justRenderer.getTotPix();
    }

    private JustRenderer justRenderer = new JustRenderer();
    public long renderRaytraced(BufferedImage img, int raytracingDepth, double gamma, int quality) {
        //return justRenderer.renderRaytraced(img, raytracingDepth);
        long tm = justRenderer.prerenderRaytraced(raytracingDepth, 0, img.getWidth(), 0, img.getHeight(), gamma, quality);
        justRenderer.postrenderRaytraced(justRenderer.getMaxColorVal(), img, 0, img.getWidth(), 0, img.getHeight(), gamma);
        return tm;
    }

    public void renderWireframe(BufferedImage img) {
        justRenderer.renderWireframe(img);
    }
}

