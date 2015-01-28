#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "GR.H"

const byte PADDING = 2;

word centerAlignedX(const char *s, word x);
void plotWindowBase(const char *title, word x0, word y0, word pageW, word pageH);

/* return count of strings with width pageW */
word plotTextWindow(const char *title, const char *text, 
                    word x0, word y0, word pageW, word pageH, bool wait = true);
void plotInputWindow(const char *title, const char *text,
                     char *buf, word bufLen,
                     word x0, word y0, word pageW, word pageH);

word getch_ext(void);

#endif