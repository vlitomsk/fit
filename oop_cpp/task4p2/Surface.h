#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <stdexcept>
#include <tuple>
#include <vector>

class BadMove : public std::exception {};

template<typename P, typename M> 
class Surface {
public:
  M move(const P&) throw (BadMove);
  std::vector<std::tuple<P, M> > lookup();
};

#endif