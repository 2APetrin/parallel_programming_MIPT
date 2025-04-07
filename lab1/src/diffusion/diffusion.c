#include <mpi.h>

#include "evaluation.h"
#include "run_info.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    run_info info;
    run_info_ctor(&info, argv);

    run_evaluation(&info);

    run_info_dtor(&info);
    MPI_Finalize();
}

