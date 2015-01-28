#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "Surface.h"

template<typename P, typename M>
class Robot {
public:
  Robot(const Surface<P, M>& s) {}
  void run(void) {}
};


#endif