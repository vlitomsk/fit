#include "PlanarSurface.h"

PlanarSurface::PlanarSurface() {}

uint PlanarSurface::lookup(Direction d) const {
  if ((d == Direction::left && x == 0) ||  (d == Direction::right && x == width - 1) ||
      (d == Direction::down && y == 0) ||  (d == Direction::up && y == height - 1))
    return notAvailable;
  
  int newX = normalizeX(x + directionDx(d));
  int newY = normalizeY(y + directionDy(d));
  if (field[newY][newX] == State::stBusy)
    return notAvailable;

  return endDistance(newX, newY);  
}

uint PlanarSurface::endDistance(int px, int py) const {
  return abs(px - endX) + abs(py - endY);
}

int PlanarSurface::normalizeX(int px) const {
    return px;
}

int PlanarSurface::normalizeY(int py) const {
    return py;
}
