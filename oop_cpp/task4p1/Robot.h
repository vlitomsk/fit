#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <iostream>
#include <cstdint>
#include "Surface.h"

class Robot {
public:
  Robot(Surface &surf) ;

  void run(void);
  
  std::ostream& printResults(std::ostream &os) const ;

private:
  Surface &surface;
  uint step;

  /* rolls back to nearest position with 1+ unmarked neighbors 
     returns direction to one of that neighbor 
     returns Direction::none if there's no such members 
     direction which decreases distance to target has higher priority */
  Direction move(void);
};


#endif