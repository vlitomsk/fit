#include "Task.h"
#include "Piece.h"
#include "Solver.h"
#include <limits>
#include <iostream>

using namespace std;

Task::~Task() {
    close_files();
}

DVector Task::run(int root_rank, double eps, std::size_t max_iters, bool &found, std::size_t &iters) {
    Solver slv(mat_sz, mat_A, vec_b);
    return slv.solve(eps, max_iters, found, iters);
}

void Task::close_files() {
    vec_fl.close();
    mat_fl.close();
}

bool Task::read_dims() {
    vec_fl >> vec_sz;
    mat_fl >> mat_sz;
    return vec_sz == mat_sz;
}

std::size_t Task::piece_start_idx(int rank) const {
    int rest = mat_sz % eff_comm_size;
    return rank < rest ? (mat_sz / comm_size + 1) * rank
                       : rest + rank * (mat_sz / eff_comm_size);
}

void Task::build_distr() {
    for (size_t i = 0; i < comm_size; ++i) {
        if (i < eff_comm_size) {
            vecdistr.push_back(Piece(0, mat_sz));
            matdistr.push_back(Piece(piece_start_idx(i), piece_start_idx(i + 1)));
        } else {
            vecdistr.push_back(Piece(0, 0));
            matdistr.push_back(Piece(0, 0));
        }
    }
}

void Task::goto_line(std::ifstream &is, std::size_t n) {
    is.seekg(std::ios::beg);
    for (size_t i = 0; i < n - 1; ++i)
        is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
}

void Task::read_vec() {
    const Piece &p = vecdistr[comm_rank];
    goto_line(vec_fl, p.from + 2);
    for (size_t i = p.from; i < p.to; ++i)
        vec_fl >> vec_b.at(i);
}

void Task::read_mat() {
   /* const Piece &p = matdistr[comm_rank];
    goto_line(mat_fl, p.from + 2);
    for (size_t j = p.from; j < p.to; ++j)
        for (size_t i = 0; i < mat_sz; ++i)
            mat_fl >> mat_A.at(i, j);*/

    /*for (size_t j = mat_A.get_piece().from; j < mat_A.get_piece().to; ++j)
        for (size_t i = 0; i < mat_sz; ++i)
            cout << "(" << i << j << ") : " << mat_A.at(i, j) << endl;
    cout << endl;*/
}
