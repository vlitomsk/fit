#ifndef _GRAPHCTL_H_
#define _GRAPHCTL_H_

#include "GR.H"
#include "WINDOW.H"
#include "Life.h"

struct Viewport {
public:
    bool contains(int64_t xc, int64_t yc) {
        return (x <= xc && xc < x + w && y <= yc && yc < y + h);
    }
    int64_t x, y, w, h;
};

class GraphCtl {
public:
    static const word
        KEY_HELP = '?',
        KEY_STEP = 'p',
        KEY_MULTISTEP = 'P',
        KEY_MOVELEFT = 'h',
        KEY_MOVEDOWN = 'j',
        KEY_MOVEUP = 'k',
        KEY_MOVERIGHT = 'l',
        KEY_DUMP = 'd',
        KEY_QUIT = 68 << 8; // F10

    GraphCtl(Life& l);
    ~GraphCtl();

    // main loop
    void start(void);

private:
    static const byte 
        STATUS_BG_COLOR = 22,
        STATUS_FG_COLOR = 52,
        BG_COLOR = 0,
        CELL_COLOR = 4;

    static const int 
        LIFE_STATUS_HEIGHT = 13,
        LIFE_STATUS_PADDING = 1,
        LIFE_VIEWPORT_WIDTH = SCREEN_WIDTH;

    static const int LIFE_VIEWPORT_Y0 = 
        LIFE_STATUS_HEIGHT + LIFE_STATUS_PADDING;

    static const int LIFE_VIEWPORT_HEIGHT = 
        SCREEN_HEIGHT - LIFE_VIEWPORT_Y0;

    static const int 
        CELLS_X = LIFE_VIEWPORT_WIDTH / CELL_GR_SIZE,
        CELLS_Y = LIFE_VIEWPORT_HEIGHT / CELL_GR_SIZE;

    Life& lifeObj;
    bool alive;
    bool fullRepaint; // set by someone to true, if it write some UI to videomemory, reset in mainloop
    Viewport viewport;
    const size_t BUFLEN;
    char *sbuffer;

    void dispatchCommand(word key);
    void actQuit(void);
    void actHelp(void);
    void actStep(void);
    void actMultistep(void);
    void actDump(void);
    void actMove(int dx, int dy);
    void drawStatus(void);
    void updateStatus(void);
    void drawCellXY(int fieldX, int fieldY);
    void drawLife(void);
    void updateLife(void);
};

#endif