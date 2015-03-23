#include <iostream>
#include "Task.h"
#include "DVector.h"
#include <vector>
#include "debug.h"
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    const static int root_rank = 0;
    int comm_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
#ifdef DEBUG
        mpi_gdb();
#endif
    Task task("bcoef", "amat");
    bool found;
    size_t iters;
    double start = MPI_Wtime();
    DVector dresult = task.run(root_rank, 1e-6, 10, found, iters);
    double end = MPI_Wtime();

    if (comm_rank == 0) {
      ofstream out;
      out.open("LOG", std::ios_base::app);
      int csize;
      MPI_Comm_size(MPI_COMM_WORLD, &csize);
      out << "size " << csize << " time " << (end - start) << endl;
      out.close();
    }

    if (!found) {
        cout << "Solution not found" << endl;
    } else {
        vector<double> result = dresult.gather(root_rank);

        int comm_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

        if (comm_rank == root_rank) {
            cout << "Solution found: " << endl;
            for (double &d : result) {
                cout << d << " ";
            }
            cout << endl;
        }
    }
    cout << "Calculation time: " << (end - start) << endl;
    MPI_Finalize();
    return 0;
}

