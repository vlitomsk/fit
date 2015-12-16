#include "inpDesc.h"
#include <stdlib.h>
#include <stdio.h>
#include "taskParams.h"

void readMatSizeFile(matDesc *, taskParams *);
void readVecFile(matDesc *md, taskParams *p);
void readSubmatFile(matDesc *md, taskParams *p);

matDesc openFileDesc(const char *path, bool *success, taskParams *p) {
  matDesc d;
  d.tp = tfileDesc;
  d.par.fd.posVec = d.par.fd.posMat = -1L;
  d.par.fd.posDim = 0L;
  d.par.fd.fd = fopen(path, "r");
  d.par.fd.cached = NULL;
  d.par.fd.veccached = NULL;
  if (!d.par.fd.fd) {
    perror("Can't open file");
  }
  *success = d.par.fd.fd != NULL;
  readMatSizeFile(&d, p);
  calcParams(p);
  readVecFile(&d, p);
  readSubmatFile(&d, p);
  return d;
}

void closeFileDesc(const matDesc *ifd) {
  if (ifd->par.fd.fd)
    fclose(ifd->par.fd.fd);
  if (ifd->par.fd.cached)
    free((ifd->par.fd).cached);
  if (ifd->par.fd.veccached)
    free(ifd->par.fd.veccached);
}

void readMatSizeFile(matDesc *md, taskParams *p) {
  inpFileDesc *ifd = &md->par.fd;
  fseek(ifd->fd, ifd->posDim, SEEK_SET);
  fscanf(ifd->fd, "%d", &p->matSize);
  ifd->dim = p->matSize;
  if (ifd->posVec == -1L)
    ifd->posVec = ftell(ifd->fd);
}

void readVecFile(matDesc *md, taskParams *p) {
  inpFileDesc *ifd = &md->par.fd;
  if (ifd->posVec == -1L)
    readMatSizeFile(md, p);

  if (!ifd->veccached) {
    ifd->veccached = (double*)malloc(sizeof(double) * ifd->dim);
    if (!ifd->veccached) {
      perror("Can't alloc veccached");
      return;
    }

    fseek(ifd->fd, ifd->posVec, SEEK_SET);
    int i;
    for (i = 0; i < ifd->dim; ++i) 
      fscanf(ifd->fd, "%lf", ifd->veccached + i);
    if (ifd->posMat == -1L)
      ifd->posMat = ftell(ifd->fd);
  }
  p->pvec = ifd->veccached;
}

void passNewLine(FILE *fd) {
  int ch;
  while ((ch = fgetc(fd)) != '\n') {}
}

void readSubmatFile(matDesc *md, taskParams *p) {
  inpFileDesc *ifd = &md->par.fd;
  int i, j;
  if (ifd->posMat == -1L) 
    readVecFile(md, p);
  
  if (!ifd->cached) {
    if (fseek(ifd->fd, ifd->posMat, SEEK_SET)) 
      perror("FSEEK ERROR");

    for (i = 0; i <= p->startY; ++i)
      passNewLine(ifd->fd);
    ifd->cached = (double*)malloc(p->blockDimY * p->matSize * sizeof(double));
    if (!ifd->cached) {
      fprintf(stderr, "p->blockDimY = %d; p->matSize = %d;", p->blockDimY,p->matSize);
      perror("Can't alloc submat cache");
      return;
    }
    for (i = 0; i < p->blockDimY * p->matSize; ++i) 
      fscanf(ifd->fd, "%lf", ifd->cached + i);
  }

  p->psubmat = ifd->cached + p->startX;
}

void readMatSizeMem(matDesc *md, taskParams *p) {
  inpMemDesc *imd = &md->par.md; 
  imd->dim = 4;
  p->matSize = imd->dim;
}

void readVecMem(matDesc *md, taskParams *p) {
  inpMemDesc *imd = &md->par.md; 
  if (!imd->dim)
    readMatSizeMem(md, p);

  if (!imd->veccached) {
    imd->veccached = (double*)malloc(sizeof(double) * imd->dim);
    if (!imd->veccached) {
      perror("Can't alloc veccached");
      return;
    }
    int i;
    for (i = 0; i < imd->dim; ++i)
      imd->veccached[i] = imd->dim + 2;
  }
  p->pvec = imd->veccached;
}

void readSubmatMem(matDesc *md, taskParams *p) {
  inpMemDesc *imd = &md->par.md; 
  if (!imd->veccached)
    readVecMem(md, p);
  if (!imd->cached) {
    imd->cached = (double*)malloc(p->blockDimY * p->matSize * sizeof(double));
    if (!imd->cached) {
      fprintf(stderr, "p->blockDimY = %d; p->matSize = %d;", p->blockDimY,p->matSize);
      perror("Can't alloc submat cache");
      return;
    }
    int i, j;
    for (j = 0; j < p->blockDimY; ++j) 
      for (i = 0; i < p->matSize; ++i)
        imd->cached[j * p->matSize + i] = (j + p->startY == i) ? 2 : 1;
  }
  p->psubmat = imd->cached + p->startX;
}

matDesc openMemDesc(bool *success, taskParams *p) {
  matDesc md;
  *success = true;
  md.tp = tmemDesc;
  md.par.md.cached = md.par.md.veccached = NULL;
  md.par.md.dim = 0;
  readMatSizeMem(&md, p);
  calcParams(p);
  readVecMem(&md, p);
  readSubmatMem(&md, p);
  return md;
}

void closeMemDesc(const matDesc *md) {
  if (md->par.md.cached)
    free(md->par.md.cached);
  if (md->par.md.veccached)
    free(md->par.md.veccached);
}

#define demuxDesc(d, p, ffile, fmem) \
  switch (d->tp) { \
    case tfileDesc: \
      ffile(d, p); \
      break; \
    case tmemDesc: \
      fmem(d, p); \
      break; \
  }

void readSubmat(matDesc *d, taskParams *p) {
  demuxDesc(d, p, readSubmatFile, readSubmatMem);
}

void readMatSize(matDesc *d, taskParams *p) {
  demuxDesc(d, p, readMatSizeFile, readMatSizeMem);
}

void readVec(matDesc *d, taskParams *p) {
  demuxDesc(d, p, readVecFile, readVecMem);
}

