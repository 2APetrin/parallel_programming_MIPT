#include <mpi.h>

#include <limits.h>
#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "eval.h"

#define POINTS_CNT UINT_MAX
#define SUMMATOR 0
#define TAG 0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank = -1, size = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned beg_pt = -1, end_pt = -1;
    get_range(POINTS_CNT, size, rank, &beg_pt, &end_pt);

    printf("intmax = %u; ", UINT_MAX);
    printf("proc%d - [%10u, %10u]\n", rank, beg_pt, end_pt);

    double a   = -1 / sqrt(2),
           b   =  1 / sqrt(2),
           tau = (b - a) / POINTS_CNT,
           sum = 0;

    for (unsigned ind = beg_pt; ind < end_pt; ++ind)
        // sum += func(tau * ind + a) * tau;
        sum += (func(tau * ind + a) + func(tau * (ind + 1) + a)) / 2 * tau;

    if (rank == SUMMATOR) {
        double *buf = malloc((size - 1) * sizeof(double));

        MPI_Request *reqs = malloc((size - 1) * sizeof(MPI_Request));
        MPI_Status *stats = malloc((size - 1) * sizeof(MPI_Status));

        for (int i = 1; i < size; ++i)
            MPI_Irecv(&buf[i-1], 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &reqs[i-1]);

        MPI_Waitall((size - 1), reqs, stats);

        for (int i = 1; i < size; ++i) {
            sum += buf[i-1];
        }

        printf("succesful recv\n");
        printf("constant  PI = 3.14159265358979323\n");
        printf("evaluated PI = %.*e\n", DBL_DECIMAL_DIG, 2*sum);

        free(stats);
        free(reqs);
        free(buf);
    }
    else {
        MPI_Ssend(&sum, 1, MPI_DOUBLE, SUMMATOR, TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
