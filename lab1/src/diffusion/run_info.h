#ifndef DIFF_RUN_INFO_H
#define DIFF_RUN_INFO_H

typedef struct {
    unsigned point_count;
    unsigned layer_count;
    unsigned begin_point;
    unsigned current_len;
    unsigned print_every_n;

    int rank;
    int size;

    double tstep;
    double xstep;

    double *buffer;
    double *prev_buffer;
} run_info;

void run_info_ctor(run_info *info, char *argv[]);

void run_info_dtor(run_info *info);

#endif // DIFF_EVALUATOIN_H
