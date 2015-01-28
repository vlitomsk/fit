#include "Surface.h"

Surface::~Surface() {
  this->freeField();
}

uint min(uint a, uint b) {
  return a < b ? a : b;
}

int directionDx(Direction d) {
  if (d == Direction::left)
    return -1;
  if (d == Direction::right)
    return 1;
  return 0;
}

int directionDy(Direction d) {
  if (d == Direction::up)
    return 1;
  if (d == Direction::down)
    return -1;
  return 0;
}

bool Surface::atStart(void) const {
  return x == startX && y == startY;
}

bool Surface::finished(void) const {
  return x == endX && y == endY;
}

void Surface::freeField(void) {
  for (uint i = 0; i < height; ++i)
    delete[] field[i];
  delete[] field;
}

void Surface::mark(uint m) {
  marks[y][x] = m;
}

uint Surface::marked(Direction d) const {
  if (lookup(d) == notAvailable)
    return notAvailable;

  return marks[normalizeY(y + directionDy(d))][normalizeX(x + directionDx(d))];
}

#include <iostream>
uint Surface::move(Direction d) throw (BadMove) {
  if (lookup(d) == notAvailable)
    throw BadMove();

  if (field[y][x] == State::stFree)
    field[y][x] = State::stWay;

  x = normalizeX(x + directionDx(d));
  y = normalizeY(y + directionDy(d));
  
  std::cout << "Now x: " << x << "; y: " << y << std::endl;

  return endDistance(x, y);
}

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

ostream& operator<<(ostream& os, const Surface& s) {
  for (uint j = 0; j < s.height; ++j) {
    for (uint i = 0; i < s.width; ++i) {
      if (j == s.y && i == s.x) {
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
    os << endl;
  }

  return os;
}

#include <algorithm>

istream& operator>>(istream& is, Surface& s) {
  string line;
  bool firstIter = true;
  vector<string> lines;
  while (getline(is, line)) {
    if (firstIter) {
      firstIter = false;
      s.width = line.length();
    }

    lines.push_back(line);
  }

  s.height = lines.size();
  s.field = new State*[s.height];
  s.marks = new uint*[s.height];

  for (size_t i = 0; i < lines.size(); ++i) {
    s.field[i] = new State[s.width];
    s.marks[i] = new uint[s.width];
    fill(s.marks[i], s.marks[i] + s.width, 0);
    string& str = lines[i];
    for (size_t j = 0; j < s.width; ++j) {
      switch (str[j]) {
        case chFree: s.field[i][j] = State::stFree; break;
        case chEnd: 
          s.endX = j;
          s.endY = i;
          s.field[i][j] = State::stEnd; break;
        case chStart: 
          s.x = s.startX = j;
          s.y = s.startY = i;
          s.field[i][j] = State::stStart; break;
        case chBusy: s.field[i][j] = State::stBusy; break;
        case chWay: s.field[i][j] = State::stWay; break;
        default: throw runtime_error("Unsupported input");
      }
    }
  }

  cout << "endX: " << s.endX << "; endY: " << s.endY << endl;

  return is;
}
