#ifndef _TORUS_SURFACE_H_
#define _TORUS_SURFACE_H_

#include "Surface.h"

class TorusSurface : public Surface {
public:
  TorusSurface();

  uint lookup(Direction d) const;

  uint endDistance(int px, int py) const;

protected:
  int normalizeX(int px) const;

  int normalizeY(int py) const;
};

#endif