#include "DVector.h"
#include <cmath>
#include <mpi.h>
#include <iostream>

using namespace std;

const double& DVector::at(std::size_t idx) const {
    return rtvec.at(idx - distr[distr_idx].from);
}

double& DVector::at(std::size_t idx) {
    return rtvec.at(idx - distr[distr_idx].from);
}

DVector DVector::lincomb(double this_coef, double v_coef, const DVector &v) const {
    DVector res(v);
    lincomb(this_coef, v_coef, v, res);
    return res;
}

const DVector& DVector::lincomb(double this_coef, double v_coef, const DVector &v, DVector &out) const {
    throwcompat(v);
    throwcompat(out);
    for (size_t i = 0; i < rtvec.size(); ++i)
        out.rtvec[i] = this_coef * rtvec[i] + v_coef * v.rtvec[i];
    return out;
}

double DVector::dot(const DVector &v) const {
    throwcompat(v);
    double sum, nodeSum = 0;
    /* could be written easy way in FP !!! */
    for (size_t i = 0; i < rtvec.size(); ++i)
        nodeSum += rtvec[i] * v.rtvec[i];

    MPI_Allreduce(&nodeSum, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    return sum;
}

double DVector::norm() const {
    return sqrt(dot(*this));
}

DVector DVector::operator+(const DVector &v) const {
    return lincomb(1, 1, v);
}

const DVector& DVector::operator+=(const DVector &v) {
    return lincomb(1, 1, v, *this);
}

DVector DVector::operator-(const DVector &v) const {
    return lincomb(1, -1, v);
}

const DVector& DVector::operator-=(const DVector &v) {
    return lincomb(1, -1, v, *this);
}

DVector DVector::operator*(double coef) const {
    return lincomb(0, coef, *this);
}

const DVector& DVector::operator *=(double coef) {
    return lincomb(0, coef, *this, *this);
}

void DVector::throwcompat(const DVector &v) const {
    if (v.distr[v.distr_idx] != distr[distr_idx]) {
        cout << v.distr[v.distr_idx].from << " " << v.distr[v.distr_idx].to << " ; " << distr[distr_idx].from << " " << distr[distr_idx].to << endl;
        throw std::invalid_argument("Different pieces!");
    }
}

std::size_t DVector::get_rtsize() const {
    return rtsize;
}

const std::vector<Piece>& DVector::get_distr() const {
    return distr;
}

const Piece& DVector::get_piece() const {
    return distr[distr_idx];
}

std::size_t DVector::get_distr_idx() const {
    return distr_idx;
}

std::vector<double> DVector::gather(int root_rank) const {
    std::vector<double> res;
    int *recvcounts = new (std::nothrow) int[2 * distr.size()];
    if (!recvcounts)
        return std::vector<double>();
    int *displs = recvcounts + distr.size();
    for (std::size_t i = 0; i < distr.size(); ++i) {
        recvcounts[i] = distr[i].len();
        displs[i] = distr[i].from;
    }

    std::vector<double> rtvec_copy(rtvec); // without const
    res.resize(rtsize);
    MPI_Gatherv(rtvec_copy.data(), rtvec_copy.size(), MPI_DOUBLE,
                res.data(), recvcounts, displs, MPI_DOUBLE, root_rank, MPI_COMM_WORLD);
    delete[] recvcounts;
    return res;
}

void DVector::set_distr_idx(std::size_t distr_idx) {
    if (distr_idx > distr.size())
        throw std::out_of_range("distr_idx too big");
    this->distr_idx = distr_idx;
}

#warning test it!
DVector& DVector::redistrib(const std::vector<Piece> &distr2, std::size_t distr2_idx) {
    if (distr2.size() != distr.size())
        throw std::invalid_argument("distr size differs from my");

    int *rcounts = new int[4 * distr.size()];
    int *rdispls = rcounts + distr.size();
    int *scounts = rdispls + distr.size();
    int *sdispls = scounts + distr.size();

#warning could be done O(n)
    // O(n^2)  :-(
    for (size_t srci = distr.size() - 1; srci >= 0; --srci) {
        for (size_t dsti = distr2.size() - 1; dsti >= 0; --dsti) {
            Piece intersc(distr[srci].intersect(distr2[dsti]));
            scounts[srci] = rcounts[dsti] = intersc.len();
            sdispls[srci] = intersc.from - distr[srci].from;
            rdispls[dsti] = intersc.from - distr2[dsti].from;
        }
    }

    vector<double> new_rtvec;
    new_rtvec.resize(distr2[distr2_idx].len());
    distr_idx = distr2_idx;
    MPI_Alltoallv(rtvec.data(), scounts, sdispls, MPI_DOUBLE,
                  new_rtvec.data(), rcounts, rdispls, MPI_DOUBLE, MPI_COMM_WORLD);
    delete[] rcounts;
    rtvec = std::move(new_rtvec);

    return *this;
}

DVector& DVector::roll_left() {
    if (distr.size() == 1)
        return *this;

    const Piece &dst = distr[distr_idx],
            &src = distr[distr_idx_cycloffset(1)];

    std::vector<double> rbuf;
    rbuf.resize(dst.len());

    MPI_Status st;
    int rank, prev_rank, next_rank, csize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &csize);
    prev_rank = (rank + csize - 1) % csize;
    next_rank = (rank + csize + 1) % csize;
    //cout << "Rank " << rank << "; sending to " << distr_idx_cycloffset(-1) << " [" << dst.from << "; " << dst.to << ") doubles; recv from " << distr_idx_cycloffset(1) << " [" << src.from << "; " << src.to << ") doubles" << endl;
    MPI_Sendrecv(rtvec.data(), dst.len(), MPI_DOUBLE, prev_rank, 0, rbuf.data(),
                 src.len(), MPI_DOUBLE, next_rank, 0, MPI_COMM_WORLD, &st);
    distr_idx = distr_idx_cycloffset(1);
    rtvec = std::move(rbuf);

    return *this;
}

const std::size_t DVector::distr_idx_cycloffset(int offset) { // sorry, dear datatypes :,-(
    return static_cast<size_t>(static_cast<long long>(distr_idx) + distr.size() + offset) % distr.size();
}
