#include <mpi.h>
#include <time.h>
#include <stdio.h>

#define TIMER 0
#define UNO_REVERSE 1
#define IT_NUM 10
#define TAG 0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start, time;
    char elem = '\a',
         rec_elem;
    if (rank == TIMER) {
        start = MPI_Wtime();
        for (int i = 0; i < IT_NUM; ++i) {
            MPI_Status status;
            MPI_Ssend(&elem,    1, MPI_CHAR, UNO_REVERSE, TAG, MPI_COMM_WORLD);
            MPI_Recv(&rec_elem, 1, MPI_CHAR, UNO_REVERSE, TAG, MPI_COMM_WORLD, &status);
        }
        time = MPI_Wtime() - start;
        printf("time for one onterraction %lg nanosec\n", 1000000000 * time / (IT_NUM * 2));
    } else if (rank == UNO_REVERSE) {
        for (int i = 0; i < IT_NUM; ++i) {
            MPI_Status status;
            MPI_Recv(&rec_elem, 1, MPI_CHAR, TIMER, TAG, MPI_COMM_WORLD, &status);
            MPI_Ssend(&elem,    1, MPI_CHAR, TIMER, TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}
