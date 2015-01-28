#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <limits>
#include <cstdint>
#include <iostream>

enum Direction {
  none = 0, left = 1, right = 2, up = 3, down = 4
};

static const char 
  chFree = '.',
  chEnd = 'X',
  chStart = 'S',
  chBusy = '#',
  chWay = '*';

int directionDx(Direction d);
int directionDy(Direction d);

uint min(uint a, uint b);

enum State {
  stFree = 0, stBusy, stStart, stEnd, stWay
};

class BadMove : public std::exception {};

class Surface {
public:
  virtual ~Surface();
  virtual uint move(Direction d) throw (BadMove);
  virtual uint lookup(Direction d) const = 0;
  virtual uint endDistance(int px, int py) const = 0;
  virtual void mark(uint m);
  virtual uint marked(Direction d) const;
  bool finished(void) const;
  bool atStart(void) const;

  friend std::ostream& operator<<(std::ostream& os, const Surface& s);
  friend std::istream& operator>>(std::istream& is, Surface& s);

  static const uint notAvailable = std::numeric_limits<uint>::max();
protected:
  virtual int normalizeX(int px) const = 0;
  virtual int normalizeY(int py) const = 0;
  virtual void freeField(void);
  int x, y;
  int endX, endY;
  int startX, startY;
  uint width, height;
  State **field;
  uint **marks;
};

std::ostream& operator<<(std::ostream& os, const Surface& s);
std::istream& operator>>(std::istream& is, Surface& s);

#endif
