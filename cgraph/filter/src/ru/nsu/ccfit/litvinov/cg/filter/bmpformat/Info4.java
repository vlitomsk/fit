package ru.nsu.ccfit.litvinov.cg.filter.bmpformat;

class Info4 {
    Info3 i3;
    long bV4RedMask;
    long bV4GreenMask;
    long bV4BlueMask;
    long bV4AlphaMask;
    long bV4CSType;
    static final int bV4EndpointsSize = 36;
    byte[] bV4Endpoints;
    long bV4GammaRed;
    long bV4GammaGreen;
    long bV4GammaBlue;
}
