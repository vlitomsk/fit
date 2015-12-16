#include "taskParams.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

int calcStartX(int commRank, int commSize, int matSize) {
  int rest = matSize % commSize;
  return  commRank < rest ? (matSize / commSize + 1) * commRank
                          : rest + commRank * (matSize / commSize);
}

void calcParams(taskParams *p) {
  p->effSize = min(p->commSize, p->matSize);
  if (p->commRank > p->matSize) {
    p->blockDimX = p->blockDimY = 0;
    return;
  }
  
  p->startY = calcStartX(p->commRank, p->effSize, p->matSize);
  p->blockDimY = calcStartX(p->commRank + 1, p->effSize, p->matSize) - p->startY;
  updParams(p);
}

void updParams(taskParams *p) {
  if (p->commRank > p->matSize)
    p->blockDimX = 0;
  int offset = (p->commRank + p->roundNo) % p->effSize;
  p->startX = calcStartX(offset, p->effSize, p->matSize);
  p->blockDimX = calcStartX(offset + 1, p->effSize, p->matSize) - p->startX;
}

