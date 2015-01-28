#include "Robot.h"
#include <cstdio>

using namespace std;

Robot::Robot(Surface &surf) : surface(surf), step(1) {};

void Robot::run(void) {
  cout << surface << endl << endl;
  while (!surface.finished()) {
    getchar();
    surface.mark(step);
    surface.move(move());
    if (surface.atStart())
      break;
    cout << surface << endl;
  } 
}

std::ostream& Robot::printResults(std::ostream &os) const {
  if (surface.atStart())
    return os << "There's no path!" << endl;
  return os << surface;
}

/* rolls back to nearest position with 1+ unmarked neighbors 
   returns direction to one of that neighbor 
   returns Direction::none if there's no such members 
   direction which decreases distance to target has higher priority */
Direction Robot::move(void) {
  bool moved = false;
  size_t notExploredCount = 0;
  Direction notExplored[4];
  const Direction directions[] = {Direction::left, Direction::right, Direction::up, Direction::down};
  Direction rollBackDir = Direction::none;

  /* find not-explored neigbors, memoize previous step in rollBackDir */
  for (Direction dir : directions) {
    uint dirMark = surface.marked(dir);
    if (!dirMark)
      notExplored[notExploredCount++] = dir;
    
    if (dirMark == step - 1) {
      rollBackDir = dir;
    }
  }

  if (notExploredCount) {
    uint minDist = Surface::notAvailable;
    size_t minDistIdx = 0;
    for (size_t i = 0; i < notExploredCount; ++i) {
      if (surface.lookup(notExplored[i]) < minDist) {
        minDist = surface.lookup(notExplored[i]);
        minDistIdx = i;
      }
    }

    ++step;
    return notExplored[minDistIdx];
  } else { 
    --step;
    cout << "Rolling back" << endl;
    return rollBackDir;
  }
}
