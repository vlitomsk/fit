#ifndef _TASK_PARAMS_H_
#define _TASK_PARAMS_H_

typedef struct {
  int matSize, blockDimX, blockDimY, startY, startX, roundNo, commRank, commSize, effSize;
  double *pvec, *psubmat;
} taskParams;

int allocParams(taskParams *p);
void freeParams(taskParams *p);
int calcStartX(int commRank, int commSize, int matSize);
void calcParams(taskParams *p);
void updParams(taskParams *p);

#endif
