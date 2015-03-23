#ifndef DMATRIX_H
#define DMATRIX_H

#include "Piece.h"
#include "DVector.h"
#include <vector>
#include <iostream>

class DMatrix
{
public:
    DMatrix() = default;
    DMatrix(std::size_t root_w, std::size_t root_h, const std::vector<Piece> &distr, std::size_t distr_idx)
        : distr(distr), rtwidth(root_w), rtheight(root_h), distr_idx(distr_idx)
    {
        const Piece &p = distr[distr_idx];
        //rtvec.resize(root_w * p.len());
        one = 1.0;
        two = 2.0;
    }

    DMatrix(const DMatrix &v) = default;
    DMatrix& operator=(const DMatrix &v) = default;
    ~DMatrix() = default;

    const double& at(std::size_t j, std::size_t i) const ;
    double& at(std::size_t j, std::size_t i);

    DVector operator*(const DVector &vector) const;

    std::size_t get_rtwidth() const;
    std::size_t get_rtheight() const;

    const std::vector<Piece>& get_distr() const;
    const Piece& get_piece() const;
private:
    double one, two;
    std::vector<Piece> distr;
    std::size_t distr_idx;
    std::size_t rtwidth, rtheight;
    //std::vector<double> rtvec;
};

#endif // DMATRIX_H
