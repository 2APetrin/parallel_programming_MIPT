#include <mpi.h>

#include <stdlib.h>
#include "evaluation.h"
#include "run_info.h"

double* init_prev_buffer(unsigned beg_pt, unsigned part_len, double xstep) {
    double *prev_buff = malloc(part_len * sizeof(double));
    for (unsigned i = 0; i < part_len; ++i)
        prev_buff[i] = u_0x((beg_pt + i) * xstep);
    if (beg_pt == 0) prev_buff[0] = u_t0(0);
    return prev_buff;
}

void init_model_params(unsigned *pnt_cnt, unsigned *layer_cnt, double *tstep,
                       double *xstep, unsigned *print_every_n, char *argv[]) {
    *pnt_cnt       = atoi(argv[1]);
    *layer_cnt     = atoi(argv[2]);
    *tstep         = atof(argv[3]);
    *xstep         = atof(argv[4]);
    *print_every_n = atoi(argv[5]);
}

void get_range(run_info *info) {
    unsigned n = info->point_count / info->size;
    if (info->point_count % info->size > (unsigned) info->rank) {
        info->begin_point = info->rank * (n + 1);
        info->current_len = n + 1;
    } else {
        unsigned tmp = (n + 1) * (info->point_count % info->size);
        info->begin_point = tmp + n * (info->rank - info->point_count % info->size);
        info->current_len = n;
    }
}

void run_info_dtor(run_info *info) {
    free(info->prev_buffer);
    free(info->buffer);
}

void run_info_ctor(run_info *info, char *argv[]) {
    init_model_params(&info->point_count, &info->layer_count, &info->tstep,
                      &info->xstep, &info->print_every_n, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &info->rank);
    MPI_Comm_size(MPI_COMM_WORLD, &info->size);

    get_range(info);

    info->buffer      = malloc(info->current_len * sizeof(double));
    info->prev_buffer = init_prev_buffer(info->begin_point,
                                         info->current_len,
                                         info->xstep);
}
