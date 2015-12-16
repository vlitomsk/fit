#ifndef _INP_DESC_H_
#define _INP_DESC_H_

#include "common.h"
#include "taskParams.h"
#include <stdio.h>

typedef enum { tfileDesc = 0, tmemDesc = 1 } descType;

typedef struct {
  int dim;
  long posDim, posVec, posMat;
  FILE *fd;
  double *cached, *veccached;
} inpFileDesc;

typedef struct {
  int dim;
  double *cached, *veccached;
} inpMemDesc;

typedef struct {
  union {
    inpFileDesc fd;
    inpMemDesc md;
  } par;
  descType tp;
} matDesc;

matDesc openFileDesc(const char *path, bool *success, taskParams *p);
void closeFileDesc(const matDesc *ifd);

matDesc openMemDesc(bool *success, taskParams *p);
void closeMemDesc(const matDesc *md);

void readSubmat(matDesc *d, taskParams *p);
void readMatSize(matDesc *d, taskParams *p);
void readVec(matDesc *d, taskParams *p);

#endif
