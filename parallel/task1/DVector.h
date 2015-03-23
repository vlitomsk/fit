#ifndef DVECTOR_H
#define DVECTOR_H

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "Piece.h"
#include <mpi.h>
#include <iostream>

class DVector
{
public:
    DVector() : distr_idx(0), rtsize(0) {
        rtvec.clear();
        distr.clear();
    }

    DVector(std::size_t rtsize, const std::vector<Piece> &distr, std::size_t distr_idx)
        : distr_idx(distr_idx), distr(distr), rtsize(rtsize)
    {
        const Piece &p = distr[distr_idx];
        rtvec.resize(p.to - p.from);
    }

    DVector(const DVector &v)
        : distr(v.distr), rtsize(v.rtsize), rtvec(v.rtvec), distr_idx(v.distr_idx)
    {
    }

    const DVector& operator=(const DVector &v) {
        distr_idx = v.distr_idx;
        distr = v.distr;
        rtsize = v.rtsize;
        rtvec = v.rtvec;

        return *this;
    }

    ~DVector() {
    }

    const double& at(std::size_t idx) const;
    double& at(std::size_t idx);

    double dot(const DVector &v) const;
    double norm() const;

    DVector lincomb(double this_coef, double v_coef, const DVector &v) const;
    const DVector& lincomb(double this_coef, double v_coef, const DVector &v, DVector &out) const;
    DVector operator+(const DVector &v) const;
    const DVector& operator+=(const DVector &v);
    DVector operator-(const DVector &v) const;
    const DVector& operator-=(const DVector &v);
    DVector operator*(double coef) const;
    const DVector& operator *=(double coef);

    void throwcompat(const DVector &v) const;

    std::size_t get_rtsize() const;
    const std::vector<Piece>& get_distr() const;
    std::size_t get_distr_idx() const;
    void set_distr_idx(std::size_t distr_idx);
    const Piece& get_piece() const;

    std::vector<double> gather(int root_rank) const;
    DVector& roll_left();

    /*void print(const std::string &title) {
        //std::vector<double> res = gather(0);
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0) {
            std::cout << title << " : ";
            for (const double &d : rtvec)
                std::cout << d << " ";
            std::cout << std::endl;
        }
    }*/

    DVector& redistrib(const std::vector<Piece> &distr, std::size_t distr_idx);
    inline DVector& redistrib(const std::vector<Piece> &distr) {
        return redistrib(distr, this->distr_idx);
    }

private:
    std::vector<Piece> distr;
    std::size_t distr_idx, rtsize;
    std::vector<double> rtvec;

    const std::size_t distr_idx_cycloffset(int offset);
};

#endif // DVECTOR_H
