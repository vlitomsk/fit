#include "Solver.h"

using namespace std;

DVector Solver::solve(double eps, std::size_t max_iters, bool &found, std::size_t &iters) {
    DVector vec_x(mat_sz, vecdistr, distr_idx);

    for (size_t i = vec_x.get_piece().from; i < vec_x.get_piece().to; ++i)
        vec_x.at(i) = 2;
    found = false;
    for (iters = 0; !found && iters < max_iters; ++iters) {
        //if (iters % 10 == 0) {
        //    cout << "iters: " << iters << endl;
        //}
        //vec_x.print("X");
        DVector vec_y(mat_A * vec_x);
        //vec_y.print("Ax");
        vec_y -= vec_b;
        //vec_y.print("Ax-b");
        DVector vec_Ay(mat_A * vec_y);
        //vec_Ay.print("Ay");
        double tau_num = vec_y.dot(vec_Ay),
               tau_denom = vec_Ay.dot(vec_Ay);
        double tau = tau_num / tau_denom;
        found = solution_ok(vec_y, eps);
        if (!found)
            vec_x.lincomb(1, -tau, vec_y, vec_x);
    }

    return vec_x;
}

bool Solver::solution_ok(DVector &vec_y, double eps) const {
    return vec_y.norm() / b_norm < eps;
}
