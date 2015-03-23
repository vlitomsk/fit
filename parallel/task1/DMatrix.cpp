#include "DMatrix.h"
#include "debug.h"
#include <mpi.h>

using namespace std;

const double& DMatrix::at(std::size_t j, std::size_t i) const {
    return (j == i ? two : one);
    //return rtvec.at((i - distr[distr_idx].from) * rtwidth + j);
}

double& DMatrix::at(std::size_t j, std::size_t i) {
    return (j == i ? two : one);
    //return rtvec.at((i - distr[distr_idx].from) * rtwidth + j);
}

DVector DMatrix::operator*(const DVector &vector) const {
    DVector v(vector);
    DVector res(this->rtheight, this->distr, this->distr_idx);
    const Piece &matpce = distr[distr_idx];

    /* it's too lazy to implement iterators & use std::fill */
    for (size_t i = matpce.from; i < matpce.to; ++i)
        res.at(i) = 0;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (size_t rounds = v.get_distr().size(); rounds > 0; --rounds) {
        const Piece &vpce = v.get_piece();
        for (size_t j = matpce.from; j < matpce.to; ++j) {
            for (size_t i = vpce.from; i < vpce.to; ++i) {
                res.at(j) += v.at(i) * this->at(i, j);
            }
        }

        if (rounds > 1)
            v.roll_left();
    }

    return res.redistrib(v.get_distr());
}

std::size_t DMatrix::get_rtwidth() const {
    return rtwidth;
}

std::size_t DMatrix::get_rtheight() const {
    return rtheight;
}

const std::vector<Piece>& DMatrix::get_distr() const {
    return distr;
}

const Piece& DMatrix::get_piece() const {
    return distr[distr_idx];
}
