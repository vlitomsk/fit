#include "WINDOW.H"
#include "GR.H"
#include <conio.h>
#include <string.h>
#include "COLORS.H"

//using namespace std;

word centerAlignedX(const char *s, word x) {
        return x - (strlen(s) >> 1) * globFontWidth();
}

void plotWindowBase(const char *title, word x0, word y0, word pageW, word pageH) {
    setColor(UICOLOR_TITLE_BG);
    byte width = globFontWidth() * pageW + PADDING * 2;
    byte fHeight = globFontHeight();
    plotRect(x0, y0, width, fHeight);
    setColor(UICOLOR_PANE);
    plotRect(x0, y0 + fHeight, width, fHeight * pageH);
    setTextBG(UICOLOR_TITLE_BG);
    setTextFG(UICOLOR_TITLE_FG);
    plotText(title, centerAlignedX(title, x0 + (width >> 1)), y0); // window title
}

const word 
    KEY_UP = 72 << 8,
    KEY_DOWN = 80 << 8,
    KEY_LEFT = 75 << 8,
    KEY_RIGHT = 77 << 8;

word getch_ext(void) {
    word ch = getch();
    if (ch == 0)
        return (byte)getch() << 8;
    else
        return ch;
}

word plotTextWindow(const char *title, const char *text, 
                    word x0, word y0, word pageW, word pageH, bool wait) 
{
    plotWindowBase(title, x0, y0, pageW, pageH);
    x0 += PADDING;
    y0 += globFontHeight();
    word w = pageW * globFontWidth();
    word h = pageH * globFontHeight();
    setTextBG(UICOLOR_PANE);
    setTextFG(UICOLOR_TEXT);
    word pageCap = pageW * pageH;

    word stringCount = strlen(text) / pageW + (strlen(text) % pageW == 0 ? 0 : 1);
    word tmp = stringCount;
    word pageCount = stringCount / pageH + (stringCount % pageH == 0 ? 0 : 1);

    for (int page = 0; page < pageCount; ++page) {
        //plotText("AHAHAHAHAHA", 0, 0);
        if (page == pageCount - 1) 
            plotRect(x0, y0, w, h);
        
        for (int i = 0; i < (page < pageCount - 1 ? pageH : stringCount); ++i) 
            plotText(text + i * pageW, x0, y0 + globFontHeight() * i, pageW);
        
        text += pageCap;
        stringCount -= pageH;
        if (page != pageCount - 1) {
            getch_ext();
        }
    }
    if (wait)
        getch_ext();

    return tmp;
}

void plotInputWindow(const char *title, const char *text,
                     char *buf, word bufLen,
                     word x0, word y0, word pageW, word pageH) 
{
    word stringCount = plotTextWindow(title, text, x0, y0, pageW, pageH, false);
    x0 += PADDING;
    y0 += globFontHeight();
    word w = pageW * globFontWidth();
    if (stringCount % pageH == 0) {
        setColor(UICOLOR_PANE);
        plotRect(x0, y0, w, pageH * globFontHeight());
    }
    setColor(UICOLOR_INPUT_BG);
    word input_x = x0;
    word input_y = y0 + (stringCount % pageH) * globFontHeight();
    plotRect(input_x, input_y, w, globFontHeight());

    memset(buf, 0, bufLen);
    setTextBG(UICOLOR_INPUT_BG);
    setTextFG(UICOLOR_TEXT);
    word ch;
    word ptr = 0;
    bool erase = false;
    while(1) {
        if (ptr <= pageW || erase) {
            plotRect(input_x, input_y, w, globFontHeight());
            erase = false;
        }
        
        if (ptr <= pageW) {
            plotText(buf, input_x, input_y);
        } else { // ptr > pageW
            plotText(buf + ptr - pageW, input_x, input_y, pageW);
        }
        ch = getch_ext();
        if (ch >= ' ' && ch <= '~') {
            if (ptr == bufLen - 2)
                continue;
            buf[ptr++] = (char)(ch & 0xff);
        } else if (ch == '\b') {
            if (ptr > 0) {
                buf[--ptr] = 0;
                erase = true;
            }
        } else if (ch == '\r') {
            break;
        }
    }
}
