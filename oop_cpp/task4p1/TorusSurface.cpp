#include "TorusSurface.h"

TorusSurface::TorusSurface() {}

uint TorusSurface::lookup(Direction d) const {
  int newX = normalizeX(x + directionDx(d)),
      newY = normalizeY(y + directionDy(d));
  if (field[newY][newX] == State::stBusy)
    return notAvailable;
  return endDistance(newX, newY);
}

uint TorusSurface::endDistance(int px, int py) const {
  return min(abs(px - endX) + abs(py - endY),
         min(abs(px + width - endX) + abs(py - endY),
             abs(px - endX) + abs(py + height - endY)));
}

int TorusSurface::normalizeX(int px) const {
  if (px < 0)
    return px + width;
  return px % width;
}

int TorusSurface::normalizeY(int py) const {
  if (py < 0)
    return py + height;
  return py % height;
}
