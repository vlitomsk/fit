#include "CylinderSurface.h"

CylinderSurface::CylinderSurface() {}

uint CylinderSurface::lookup(Direction d) const {
  if ((y == 0 && d == Direction::down) || (y == height - 1 && d == Direction::up))
    return notAvailable;
  int newX = normalizeX(x + directionDx(d));
  int newY = normalizeY(y + directionDy(d));
  if (field[newY][newX] == State::stBusy)
    return notAvailable;
  return endDistance(newX, newY);
}

uint CylinderSurface::endDistance(int px, int py) const {
  return min(abs(px - endX) + abs(py - endY),
             abs(px + width - endX) + abs(py - endY));
}

int CylinderSurface::normalizeX(int px) const {
  if (px < 0)
    return px + width;
  return px % width;
}

int CylinderSurface::normalizeY(int py) const {
    return py;
}