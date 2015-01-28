#ifndef _PLANAR_SURFACE_H_
#define _PLANAR_SURFACE_H_

#include "Surface.h"

class PlanarSurface : public Surface {
public:
  PlanarSurface();

  uint lookup(Direction d) const;

  uint endDistance(int px, int py) const;
protected:
  int normalizeX(int px) const;

  int normalizeY(int py) const;
};

#endif
