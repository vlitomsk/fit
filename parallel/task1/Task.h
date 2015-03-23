#ifndef TASK_H
#define TASK_H

#include <mpi.h>
#include "DMatrix.h"
#include "DVector.h"
#include "Solver.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>

class Task
{
public:
    Task(const std::string &vec_filepath, const std::string &mat_filepath)
        : vec_fl(vec_filepath), mat_fl(mat_filepath), root_rank(0)
    {
        if (!mat_fl.is_open() || !vec_fl.is_open()) {
            close_files();
            throw std::invalid_argument("Some files doesn't exist"); // all resouces are freed/closed
        }

        MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

        for (size_t i = 0; i < comm_size; ++i) {
            if (comm_rank == i && !read_dims()) {
                close_files();
                throw std::invalid_argument("Mat&vec dimensions differs"); // all resources are freed/closed
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        eff_comm_size = std::min(mat_sz, static_cast<size_t>(comm_size));
        build_distr();
        vec_b = DVector(mat_sz, vecdistr, comm_rank);
        mat_A = DMatrix(mat_sz, mat_sz, matdistr, comm_rank);

        for (size_t i = 0; i < comm_size; ++i) {
            if (comm_rank == (i + 1) % comm_size)
                read_vec();
            if (comm_rank == i)
                read_mat();
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    /*void print_distr() {
        for (const Piece &p : distr) {
            std::cout << "[" << p.from << "; " << p.to << ")" << std::endl;
        }
    }*/

    Task(const Task &t) = delete;
    Task& operator=(const Task &t) = delete;

    ~Task();
    virtual DVector run(int root_rank, double eps, std::size_t max_iters, bool &found, std::size_t &iters);

private:
    std::vector<Piece> matdistr, vecdistr;
    int comm_rank, comm_size, eff_comm_size;
    const int root_rank;
    std::ifstream vec_fl, mat_fl;
    std::size_t mat_sz, vec_sz;

    DVector vec_b;
    DMatrix mat_A;

    std::size_t piece_start_idx(int rank) const;

    void build_distr();

    void goto_line(std::ifstream &is, std::size_t n) ;
    bool read_dims();
    void read_vec();
    void read_mat();

    void close_files();
};

#endif // TASK_H
