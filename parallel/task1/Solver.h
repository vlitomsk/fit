#ifndef SOLVER_H
#define SOLVER_H

#include "DMatrix.h"
#include "DVector.h"
#include <cstdlib>

class Solver
{
public:
    Solver(std::size_t mat_sz, const DMatrix &mat_A, const DVector &vec_b)
        : mat_A(mat_A), vec_b(vec_b), vecdistr(vec_b.get_distr()), matdistr(mat_A.get_distr()),
          distr_idx(vec_b.get_distr_idx()), b_norm(vec_b.norm()), mat_sz(mat_sz)
    {
    }

    DVector solve(double eps, std::size_t max_iters, bool &found, std::size_t &iters);
private:
    DMatrix mat_A;
    DVector vec_b;
    const std::vector<Piece> &vecdistr, &matdistr;
    const std::size_t distr_idx, mat_sz;
    const double b_norm;

    bool solution_ok(DVector &vec_y, double eps) const;
};

#endif // SOLVER_H
