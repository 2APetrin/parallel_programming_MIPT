#include <math.h>

void get_range(unsigned arr_len, int proc_cnt, int rank, unsigned *beg, unsigned *end) {
    unsigned n = arr_len / proc_cnt;
    if (arr_len % proc_cnt > rank) {
        *beg = rank * (n + 1);
        *end = *beg + (n + 1);
        return;
    }
    unsigned tmp = (n + 1) * (arr_len % proc_cnt);
    *beg = tmp + n * (rank - arr_len % proc_cnt);
    *end = *beg + n;
}

double func(double x) {
    return 1 / sqrt((1 - x * x));
}
