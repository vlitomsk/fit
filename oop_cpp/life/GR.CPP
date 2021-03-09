#include <dos.h>
#include <mem.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "GR.H"

byte far *VGA = (byte far*)0xA0000000;

#define vgaOffset(x, y) ( VGA + ((y) << 8) + ((y) << 6) + (x))

Font *g_font;
byte g_color, g_textFG, g_textBG;
unsigned gw_color;

const byte GRAPH_MODE = 0x13;
const byte TEXT_MODE = 0x3;

// return value : status OK
byte loadBMP(const char *path, BitmapImage *img) {
    byte statusOk = 1;
    FILE *fd = fopen(path, "rb");
    if (!fd) {
        printf("Can't open file %s\n", path);
        statusOk = 0;
    }
    byte *buf = (byte*)malloc(15); // BITMAP FILE HEADER
    if (!buf) {
        printf("Can't alloc buf\n");
        statusOk = 0;
    }

    dword filesize;
    
    // parsing BITMAP FILE HEADER
    if (statusOk) {
        memset(buf, 0, 14);
        if (fread(buf, 1, 14, fd) < 14) {
            printf("Can't read BMP FILE HEADER\n");
            statusOk = 0;
        }
    }

    if (statusOk) {
        if (*(word*)(buf) != 0x4d42 ||
            *(word*)(buf + 0x6) != 0x0000) {
            printf("Some BMP control bytes are BAD\n");
            statusOk = 0;
        }
    }

    if (statusOk) {
        filesize = *(dword*)(buf + 0x2);
        byte *buf_new = (byte*)realloc(buf, filesize);
        if (!buf_new) {
            printf("Can't realloc buf\n");
            statusOk = 0;
        } else
            buf = buf_new;
    }

    if (statusOk) {
        if (fread(buf + 14, 1, filesize - 14, fd) < filesize - 14) {
            printf("Can't read all file");
            statusOk = 0;
        }
    }
    
    // parsing BITMAP INFO
    // only bitmapinfo v4 is supported
    if (statusOk) {
        dword structSize = *(dword*)(buf + 0xe);
        if (structSize != 40 && structSize != 108 && structSize != 124) {
            printf("Your BMP has version != v3 or v4 or v5\n");
            statusOk = 0;
        }
    }
    
    dword pixDataOffset;
    if (statusOk) {
        pixDataOffset = *(dword*)(buf + 0x0a);
        img->width = *(dword*)(buf + 0x12);
        img->lineSize = (img->width & 3) ? ((img->width & 0xFFFFFFFC) + 4) : (img->width);
        img->height = *(dword*)(buf + 0x16);
        img->lineOrder = *(dword*)(buf + 0x16) >> 31;
        img->bitsPerPixel = *(word*)(buf + 0x1c);
        if (img->bitsPerPixel != 8) {
            printf("Your BMP has != 8 bits/pixel\n");
            statusOk = 0;
        }
    }

    img->bitmap = 0;
    if (statusOk) {
        img->bitmap = (byte*)malloc(img->width * img->height);
        if (!img->bitmap) {
            printf("Can't alloc memory for bitmap\n");
            statusOk = 0;
        }
    }

    if (statusOk) {
        memcpy(img->bitmap, buf + pixDataOffset, img->width * img->height);
    }
    
    fclose(fd);
    free(buf);
    if (!statusOk)
        free(img->bitmap);

    return statusOk;
}

void releaseBMP(const BitmapImage *img) {
    free(img->bitmap);
}

void plotBMP(
  const BitmapImage *img,
  int scrX, int scrY,
  int x0, int y0,
  int w, int h)
{
    int i, j;
    byte pix;

    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            pix = (img->bitmap)[(i + y0) * img->lineSize + j + x0] ? g_textBG : g_textFG;
            *vgaOffset(scrX + j, scrY + h - i - 1) = pix;
        }
    }
}

void setVideoMode(byte mode) {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
    int86(0x10, &regs, &regs);
}

// loads 6x12 font
byte loadFont(const char *path, Font *fnt) {
    byte statusOk = 1;
    FILE *fd = fopen(path, "rb");
    if (!fd) {
        printf("Can't open font %s\n", path);
        statusOk = 0;
    }

    byte *buf;
    if (statusOk) {
        buf = (byte*)malloc(4);
        if (!buf) {
            printf("Can't alloc memory\n");
            statusOk = 0;
        }
    }

    if (statusOk) {
        if (fread(buf, 1, 4, fd) < 4) {
            printf("Bad font file format\n");
            statusOk = 0;
        }
    }

    word BMPpathlen;
    if (statusOk) {
        fnt->width = buf[0];
        fnt->height = buf[1];
        BMPpathlen = *(word*)(buf + 0x2);
        byte *buf_new = (byte*)realloc(buf, BMPpathlen + 1);
        if (!buf_new) {
            printf("Can't realloc memory\n");
            statusOk = 0;
        } else {
            buf = buf_new;
        }
    }

    if (statusOk) {
        if (fread(buf, 1, BMPpathlen, fd) < BMPpathlen) {
            printf("Can't read BMP path for font\n");
            statusOk = 0;
        }
        buf[BMPpathlen] = 0; // null-terminated string
    }

    if (statusOk) {
        if (!loadBMP((char*)buf, &(fnt->bitmap))) {
            printf("Can't load bitmap for font\n");
            statusOk = 0;
        }
    }
    
    fclose(fd);
    free(buf);
    return statusOk;
}

void releaseFont(const Font *fnt) {
    releaseBMP(&(fnt->bitmap));
}

byte initGraph(void) {
    setVideoMode(GRAPH_MODE);
    g_font = new Font;
    return loadFont("C:\\TERMINUS.FNT", g_font);
}

void closeGraph(void) {
    setVideoMode(TEXT_MODE);
    releaseFont(g_font);
    delete g_font;
}

byte globFontWidth(void) {
    return g_font->width;
}

byte globFontHeight(void) {
    return g_font->height;
}

void setTextFG(byte color) {
    g_textFG = color;
}

void setTextBG(byte color) {
    g_textBG = color;
}

// text to put on bmp (without head&tail quotes):
// " !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
// font: terminus 12px (letter size 6x12 pxpx)
// white background, black foreground
// image size: 570x12
// save as BMP v4, grayscale 8-bit pallette supported.
// Then make .fnt file which (see typedef struct Font in GR.H)
byte plotChar(const Font* fnt, char ch, int x, int y) {
    if (ch < ' ' || ch > '~')
        return 0;
    plotBMP(&(fnt->bitmap), x, y,
            fnt->width * (ch + (ch == ' ' ? 0 : 1) - ' '), 0,
            fnt->width, fnt->height);
    return 1;
}

byte plotChar(char ch, int x, int y) {
    return plotChar(g_font, ch, x, y);
}

void plotText(const Font *fnt, const char *text, int x, int y, int len) {
    byte newLines = 0;
    byte strPosition = 0;
    for (int i = 0; i < len; ++i) {
        if (text[i] == 0)
            break;
        if (text[i] == '\n') {
            strPosition = 0;
            newLines++;
        }
        strPosition += plotChar(fnt, text[i], x + strPosition * g_font->width,
                                              y + newLines * g_font->height);
    }
}

void plotText(const char *text, int x, int y) {
    plotText(g_font, text, x, y, strlen(text));
}

void plotText(const char *text, int x, int y, int len) {
    plotText(g_font, text, x, y, len);
}

void setColor(byte color) {
    g_color = color;
    gw_color = color | ((unsigned)color << 8);
}

void plotHLine(int y, int x0, int x1) {
    int i;
    int len = x1 - x0;
    register unsigned far *offset = (unsigned far*)vgaOffset(x0, y);
    for (i = 0; i < (len >> 1); ++i) {
        *offset = gw_color;
        offset++;
    }
    if (len & 1) {
        *vgaOffset(x1 - 1, y) = g_color;
    }
}

void plotRect(int x0, int y0, int w, int h) {
    for (int i = 0; i < h; ++i)
        plotHLine(y0 + i, x0, x0 + w);
}

void plotVLine(int x, int y0, int y1) {
    register byte far *offset = vgaOffset(x, y0);
    for (; y0 < y1; ++y0) {
        *offset = g_color;
        offset += SCREEN_WIDTH;
    }
}

void plotPixel(int x, int y) {
    *vgaOffset(x, y) = g_color;
}

/*
........
.######.
.######.
.######.
.######.
.######.
.######.
........
*/
void plotCell(int x, int y) {
    for (int i = 0; i < 6; ++i) 
        plotHLine(y + 1 + i, x + 1, x + 7);
}
