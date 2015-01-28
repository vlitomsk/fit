#include "GR.H"
#include "WINDOW.H"
#include <conio.h>

int main(int argc, char *argv[]) {
    initGraph();
    //draw();
    char buf[200];

    plotInputWindow("Input", "Your name:", buf, 200, 2, 2, 20, 10);
    //plotTextWindow("Info", "This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!This is second window!", 5, 5, 20, 10);
    //getch();
    plotTextWindow("Info", buf, 100, 100, 10, 10);
    getch();
    closeGraph();
    return 0;
}
