#ifndef _GR_H_
#define _GR_H_

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;
const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

/* If byte returned anywhere, that is status:
    0 ~ fail
    1 ~ success
*/

/* Inits graphic mode 0x13 (320x200 px, 8-bit VGA color)
  loads Terminus font from C:\TERMINUS.FNT */
extern byte initGraph(void);

/* Getters */
extern byte globFontWidth(void);
extern byte globFontHeight(void);

void setTextFG(byte color);
void setTextBG(byte color);

/* Releases all resources from initGraph()
   returns to text mode 0x3 */
extern void closeGraph(void);

/* SOME ELEMENTARY FUNCTIONS */
extern void setColor(byte color);

extern void plotPixel(int x, int y);

/* plots Horizontal line */
extern void plotHLine(int y, int x0, int x1);

/* plots Vertical line */
extern void plotVLine(int x, int y0, int y1);

void plotRect(int x0, int y0, int w, int h);

const byte CELL_GR_SIZE = 8; // width & height of cell
/*
draws 8x8 cell
*/
void plotCell(int x, int y);

/* FUNCTIONS WHICH USE BITMAPS */
/* Only BMP v4, grayscale 8-bit pallette is supported */

typedef struct {
    dword width;
    dword lineSize;
    dword height;
    byte lineOrder;
    byte *bitmap;
    dword bitsPerPixel;
} BitmapImage;

typedef struct {
    byte width;
    byte height;
    BitmapImage bitmap;
} Font; // it's just bitmap exploded into width*height pieces

extern byte loadBMP(const char *path, BitmapImage *img);
extern void releaseBMP(const BitmapImage *img);

/* scrX, scrY -- where (x0, y0) of bitmap appears at 
   w, h -- size of bitmap piece to appear */
extern void plotBMP(const BitmapImage *img, int scrX, int scrY, int x0, int y0, int w, int h);

extern byte loadFont(const char *path, Font *fnt);
extern void releaseFont(const Font *fnt);

/* Returns fail if [ch] is not in { ' ' , ... , '~' } */
extern byte plotChar(const Font* fnt, char ch, int x, int y);

/* Use global loaded font (Terminus 6). See plotChar(const Font*, char, int, int) */
extern byte plotChar(char ch, int x, int y);

/* Does not return anything unlike plotChar(..), it just bypasses illegal chars */
extern void plotText(const Font *fnt, const char *text, int x, int y, int len) ;

/* Use global loaded font (Terminus 6). See plotText(const Font*, const char*, int, int) */
extern void plotText(const char *text, int x, int y);

extern void plotText(const char *text, int x, int y, int len);

#endif