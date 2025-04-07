#include <mpi.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "evaluation.h"
#include "run_info.h"

#define SUMMATOR 0
#define TAG 0

double u_0x(double x) { return 0; }
double u_t0(double t) { return t*t / 5000; }

double heterogenity(double x, double t) { return 0; }

double evaluate_new_value(double fval,  double l,
                          double m,     double r,
                          double tstep, double xstep) {
    double tmp = tstep / (2 * xstep * xstep),
           fst = tmp * (r - 2 * m + l);
    double snd = (r - l) / (2 * xstep);
    return m + (fst - snd) * tstep + fval * tstep;
}

void fill_middle_buffer(run_info *info, int lnum) {
    for (unsigned ind = 1; ind < info->current_len - 1; ++ind) {
        double r = info->prev_buffer[ind + 1],
               m = info->prev_buffer[ind],
               l = info->prev_buffer[ind - 1],
               fval = heterogenity((info->begin_point + ind) * info->xstep,
               lnum * info->tstep);
        info->buffer[ind] = evaluate_new_value(fval, l, m, r,
                                               info->tstep, info->xstep);
    }
}

void get_range_for_print(unsigned arr_len, int proc_cnt, int rank,
                         unsigned *beg, unsigned *len) {
    unsigned n = arr_len / proc_cnt;
    if (arr_len % proc_cnt > rank) {
        *beg = rank * (n + 1);
        *len = n + 1;
    } else {
        unsigned tmp = (n + 1) * (arr_len % proc_cnt);
        *beg = tmp + n * (rank - arr_len % proc_cnt);
        *len = n;
    }
}

void dump_step_result(const run_info *info, int lnum) {
    if (info->rank == SUMMATOR) {
        double *buf = malloc(info->point_count * sizeof(double));

        MPI_Request *reqs = malloc((info->size - 1) * sizeof(MPI_Request));
        MPI_Status *stats = malloc((info->size - 1) * sizeof(MPI_Status));

        memcpy(buf, info->buffer, info->current_len * sizeof(double));

        for (int i = 1; i < info->size; ++i) {
            unsigned beg_pt = 0, len = 0;
            get_range_for_print(info->point_count, info->size, i, &beg_pt, &len);
            MPI_Irecv(&buf[beg_pt], len, MPI_DOUBLE, i,
                       TAG, MPI_COMM_WORLD, &reqs[i-1]);
        }

        MPI_Waitall((info->size - 1), reqs, stats);

        for (unsigned x = 0; x < info->point_count - 1; ++x)
            printf("%lf ", buf[x]);
        printf("%g\n", buf[info->point_count - 1]);
    } else {
        MPI_Ssend(info->buffer, info->current_len, MPI_DOUBLE,
                  SUMMATOR, TAG, MPI_COMM_WORLD);
    }
}

void fill_borders(run_info *info, double lval, double rval, unsigned lnum) {
    double tmp_0 = info->begin_point + info->current_len - 1;
    double tmp_1 = info->begin_point;
    if (info->rank == info->size - 1)
        info->buffer[info->current_len - 1] = rval;
    else {
        info->buffer[info->current_len - 1] = evaluate_new_value(
            heterogenity(tmp_0 * info->xstep, lnum * info->tstep),
            info->prev_buffer[info->current_len - 2],
            info->prev_buffer[info->current_len - 1],
            rval, info->tstep, info->xstep);
    }
    if (info->rank == 0)
        info->buffer[0] = u_t0(lnum * info->tstep);
    else {
        info->buffer[0] = evaluate_new_value(
            heterogenity(tmp_1 * info->xstep, lnum * info->tstep),
            lval,
            info->prev_buffer[0],
            info->prev_buffer[1], info->tstep, info->xstep);
    }
}

void run_evaluation(run_info *info) {
    for (int lnum = 1; lnum < info->layer_count; ++lnum) {
        double lval = NAN,
               rval = NAN;
        int async_count = 0;

        MPI_Request reqs[4];
        MPI_Status stats[4];

        if (info->rank == 0) lval = u_t0(lnum * info->tstep);
        else {
            MPI_Isend(&info->prev_buffer[0], 1, MPI_DOUBLE, info->rank - 1,
                       TAG, MPI_COMM_WORLD, &reqs[async_count++]);
            MPI_Irecv(&lval,                 1, MPI_DOUBLE, info->rank - 1,
                       TAG, MPI_COMM_WORLD, &reqs[async_count++]);
        }
        if (info->rank == info->size - 1) rval = 0;
        else {
            MPI_Isend(&info->prev_buffer[info->current_len - 1], 1, MPI_DOUBLE,
                       info->rank + 1, TAG, MPI_COMM_WORLD, &reqs[async_count++]);
            MPI_Irecv(&rval,                                  1, MPI_DOUBLE,
                       info->rank + 1, TAG, MPI_COMM_WORLD, &reqs[async_count++]);
        }
        fill_middle_buffer(info, lnum);
        MPI_Waitall(async_count, reqs, stats);
        fill_borders(info, lval, rval, lnum);

        if (lnum % info->print_every_n == 0) dump_step_result(info, lnum);

        double *tmp = info->prev_buffer;
        info->prev_buffer = info->buffer;
        info->buffer = tmp;
    }
}

