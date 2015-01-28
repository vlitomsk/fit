#ifndef _PLANAR_SURFACE_H_
#define _PLANAR_SURFACE_H_

#include "Surface.h"
#include <string>
#include <iostream>

/***********************************/
/* Planar surface specialization   */
/***********************************/

static const char 
  chFree = '.',
  chEnd = 'X',
  chStart = 'S',
  chBusy = '#',
  chWay = '*';
enum State {
  stFree = 0, stBusy, stStart, stEnd, stWay
};

struct Point2d {
  Point2d() {}
  Point2d(int px, int py) : x(px), y(py) {}
  bool operator==(const Point2d& p) const { 
    return p.x == x && p.y == y;
  }
  void operator=(const Point2d& p) {
    x = p.x;
    y = p.y;
  }
  int x, y;
};

typedef Surface<Point2d, uint> PlanarSurface;

template<>
class Surface<Point2d, uint> {
public:
  Surface() : dirty(true) {}

  uint move(const Point2d& dst) throw (BadMove) {
    if (dirty)
      refreshLookup();
    for (auto tp : lookupResult) 
      if (dst == std::get<0>(tp)) {
        pos = dst;
        dirty = true;
        return endDistance(pos.x, pos.y);
      }
    
    throw BadMove();
  }

  std::vector<std::tuple<Point2d, uint> > lookup(void) {
    refreshLookup();
    return std::vector<std::tuple<Point2d, uint> >(lookupResult);
  }

  friend std::ostream& operator<<(std::ostream&, const PlanarSurface&);
  friend std::istream& operator>>(std::istream&, PlanarSurface&);
private:
  std::vector<std::tuple<Point2d, uint> > lookupResult; // last lookup result
  Point2d pos, startPos, endPos;
  uint width, height; 
  bool dirty;
  State **field;

  uint endDistance(int px, int py) const {
    return abs(pos.x - endPos.x) + abs(pos.y - endPos.y);
  }

  bool clips(int px, int py) const {
    return px >= 0 && py >= 0 && px < width && py < height;
  }

  void refreshLookup(void) {
    lookupResult.clear();
    char dxs[] = { -1, 1,  0, 0 },
         dys[] = {  0, 0, -1, 1 };
    for (size_t i = 0; i < 4; ++i) {
      int px = pos.x + dxs[i], 
          py = pos.y + dys[i];
      if (clips(px, py) && field[py][px] != State::stBusy)
        lookupResult.push_back(std::tuple<Point2d, uint>(Point2d(px, py), endDistance(px, py)));
    }

    dirty = false;
  }
};

std::ostream& operator<<(std::ostream& os, const PlanarSurface& s) {
  for (uint j = 0; j < s.height; ++j) {
    for (uint i = 0; i < s.width; ++i) {
      if (j == s.pos.y && i == s.pos.x) {
        os << "@";
        continue;
      }
      switch (s.field[j][i]) {
        case State::stFree: os << chFree; break;
        case State::stEnd: os << chEnd; break;
        case State::stStart: os << chStart; break;
        case State::stBusy: os << chBusy; break;
        case State::stWay: os << chWay; break;
        default: os << "Not supported yet!!"; break;
      }
    }
    os << std::endl;
  }

  return os;
}

std::istream& operator>>(std::istream& is, PlanarSurface& s) {
  std::string line;
  bool firstIter = true;
  std::vector<std::string> lines;
  while (std::getline(is, line)) {
    if (firstIter) {
      firstIter = false;
      s.width = line.length();
    }

    lines.push_back(line);
  }

  s.height = lines.size();
  s.field = new State*[s.height];

  for (size_t i = 0; i < lines.size(); ++i) {
    s.field[i] = new State[s.width];
    std::string& str = lines[i];
    for (size_t j = 0; j < s.width; ++j) {
      switch (str[j]) {
        case chFree: s.field[i][j] = State::stFree; break;
        case chEnd: 
          s.endPos = Point2d(j, i);
          s.field[i][j] = State::stEnd; break;
        case chStart: 
          s.pos = Point2d(j, i);
          s.startPos = Point2d(j, i);
          s.field[i][j] = State::stStart; break;
        case chBusy: s.field[i][j] = State::stBusy; break;
        case chWay: s.field[i][j] = State::stWay; break;
        default: throw std::runtime_error("Unsupported input");
      }
    }
  }

  std::cout << "endX: " << s.endPos.x << "; endY: " << s.endPos.y << std::endl;

  return is;
}

#endif