#ifndef _CYLINDER_SURFACE_H_
#define _CYLINDER_SURFACE_H_

#include "Surface.h"

class CylinderSurface : public Surface {
public:
  CylinderSurface();

  uint lookup(Direction d) const ;

  uint endDistance(int px, int py) const ;

protected:
  int normalizeX(int px) const;

  int normalizeY(int py) const;
};

#endif
