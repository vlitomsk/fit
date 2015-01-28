#include "GraphCtl.h"
#include <string>
#include <conio.h>
#include <cstdio>
#include <set>

using namespace std;

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

GraphCtl::GraphCtl(Life& l) 
    : lifeObj(l), alive(true), fullRepaint(true), BUFLEN(256), sbuffer(new char[BUFLEN])
{
    viewport.x = viewport.y = 0;
    viewport.w = min(CELLS_X, l.getWidth());
    viewport.h =  min(CELLS_Y, l.getHeight());
}

GraphCtl::~GraphCtl() {
    delete[] sbuffer;
    closeGraph();
}

// main loop
void GraphCtl::start(void) {
    initGraph();

    while (alive) {
        if (fullRepaint) {
            drawStatus();
            drawLife();
            fullRepaint = false;
        } else {
            drawStatus();
            updateLife();
        }
        word ch = getch_ext();
        dispatchCommand(ch);
    }
    //getch();
    closeGraph();
}

void GraphCtl::dispatchCommand(word key) {
    switch (key) {
        case KEY_HELP:
            actHelp();
            break;
        case KEY_STEP:
            actStep();
            break;
        case KEY_MULTISTEP:
            actMultistep();
            break;
        case KEY_MOVELEFT:
            actMove(-1, 0); 
            break;
        case KEY_MOVERIGHT:
            actMove(1, 0);
            break;
        case KEY_MOVEUP:
            actMove(0, -1); 
            break;
        case KEY_MOVEDOWN:
            actMove(0, 1);
            break;
        case KEY_DUMP:
            actDump();
            break;
        case KEY_QUIT:
            actQuit();
            break;
        default:
            break;
    }
}

void GraphCtl::actQuit(void) {
    alive = false;
}

void GraphCtl::actHelp(void) {
    plotTextWindow("Help", 
        "Conway's game of life. Press: "
        " <F10>  to quit               "
        " <hjkl> to move around        "
        " <p>    to simulate 1 step    "
        " <P>    to simulate N steps   "
        " <d>    to dump current state "
        " <?>    to show this help     ", 65, 30, 30, 7);
    fullRepaint = true;
}

void GraphCtl::actStep(void) {
    lifeObj.step();
    //fullRepaint = true; // temp
}

void GraphCtl::actMultistep(void) {
    int steps;
    do {
        plotInputWindow("Multistep", "Enter steps count to simulate:", sbuffer, 65, 30, 30, 30, 2);
    } while (sscanf(sbuffer, "%d", &steps) != 1);
    lifeObj.step(steps);
    fullRepaint = true;
}

void GraphCtl::actDump(void)  {
    plotInputWindow("Dump", "Enter dump path:", sbuffer, BUFLEN, 65, 30, 30, 2);
    //string s = out_file;
    lifeObj.dump(string(sbuffer));
    fullRepaint = true;
}

// makes x in [A;B]
void toBounds(int64_t* x, int64_t A, int64_t B) {
    if (*x < A)
        *x = A;
    if (*x > B)
        *x = B;
}

void GraphCtl::actMove(int dx, int dy) {
    if (dx && lifeObj.getWidth() > CELLS_X) {
        viewport.x += (int64_t)dx; 
        toBounds(&(viewport.x), 0, lifeObj.getWidth() - viewport.w - 1);
        fullRepaint = true;
    }
    if (dy && lifeObj.getHeight() > CELLS_Y) {
        viewport.y += (int64_t)dy; 
        if (dy == 1)
            viewport.y++;
        else if (dy == 2)
            viewport.y--;
    
        toBounds(&(viewport.y), 0, lifeObj.getHeight() - viewport.h - 1);
        fullRepaint = true;
    }
}

void GraphCtl::drawStatus(void) {
    setTextFG(STATUS_FG_COLOR);
    setTextBG(STATUS_BG_COLOR);
    setColor(STATUS_BG_COLOR);
    plotRect(0, 0, SCREEN_WIDTH, LIFE_STATUS_HEIGHT);
    plotText(" [Life game] [Press ? for help]", 0, 0);
}

void GraphCtl::drawCellXY(int fieldX, int fieldY) {
    if (viewport.contains(fieldX, fieldY)) {
        setColor(lifeObj.get(fieldX, fieldY) ? CELL_COLOR : BG_COLOR);
        plotCell((fieldX - viewport.x) * CELL_GR_SIZE, 
                 LIFE_VIEWPORT_Y0 + (fieldY - viewport.y) * CELL_GR_SIZE);
    }
}

void GraphCtl::drawLife(void) {
    setColor(BG_COLOR);
    plotRect(0, LIFE_VIEWPORT_Y0, LIFE_VIEWPORT_WIDTH, LIFE_VIEWPORT_HEIGHT);
    setColor(CELL_COLOR);

    for (size_t i = viewport.x; i < viewport.x + viewport.w; ++i) 
        for (size_t j = viewport.y; j < viewport.y + viewport.h; ++j) 
            drawCellXY(i, j);
}

void GraphCtl::updateLife(void) {
    const set<LifeCoord>& lst = lifeObj.getDifference();
    for (set<LifeCoord>::const_iterator c = lst.begin(); c != lst.end(); ++c) 
        drawCellXY(c->x, c->y);
}
