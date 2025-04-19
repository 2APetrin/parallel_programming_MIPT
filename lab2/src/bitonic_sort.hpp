#ifndef BITONIC_SORT_HPP
#define BITONIC_SORT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <iterator>

// I dunno how to move it into namespace
#include <pthread.h>

namespace bitonic_sort {

    namespace detail {

        enum class SEQ_TYPE {
            INCREASE,
            DECREASE
        };

        template<typename it>
        struct sort_arg {
            pthread_barrier_t *barrier;
            it first;
            it last;
            int rank;
            int threadc;
            int arr_len;

        public:
            sort_arg(pthread_barrier_t *bar, it fst, it lst,
                     int rank_, int threadc_, int arr_len_) :
                     barrier(bar), first(fst), last(lst),
                     rank(rank_), threadc(threadc_), arr_len(arr_len_) {}
        };

        template<typename comp, typename it>
        void sort_step(it start, it stop, int comp_len, SEQ_TYPE dir, int sort_size) {
            it curr = start;
            // std::string a = "start=" + std::to_string(*start) + "\n";
            // std::cout << a;
            int pos = 0;
            while (curr != stop) {
                it half = curr + (comp_len / 2);
                it end  = curr + comp_len;
                pos += comp_len;
                while (half != end) {
                    // std::cout << std::string("sort_step: ") + "rank=" + std::to_string(rank) + " half=" + std::to_string(*half) + " comp_len=" + std::to_string(comp_len) + "\n";
                    bool cond_comp = comp()(*curr, *half);
                    bool cond_dir  = (dir == SEQ_TYPE::DECREASE);
                    // bool cond = (cond_comp && cond_dir) || !(cond_comp || cond_dir); // @TODO rewrite
                    bool cond = (cond_comp == cond_dir);
                    if (cond) {
                        std::iter_swap(curr, half);
                        // std::cout << "rank=" + std::to_string(rank) + " swapped" + "\n";
                    }
                    ++curr; ++half;
                }
                curr = end;
                if (pos >= sort_size) {
                    if (SEQ_TYPE::INCREASE == dir) dir = SEQ_TYPE::DECREASE;
                    else dir = SEQ_TYPE::INCREASE;
                    pos = 0;
                }
            }
        }

        // may be not inline
        inline std::pair<int, int> get_range(int threadc, int arr_len, int rank, int comp_len) {
            int part_len = arr_len / threadc;
            if (comp_len <= part_len) {
                int beg = rank * part_len;
                int end = beg + part_len;
                return {beg, end};
            } else {
                int beg = rank * comp_len;
                int end = beg + comp_len;
                return {beg, end};
            }
        }

        template<typename comp, typename it>
        void* sort_executor(void *args) {
            sort_arg<it>* arg = static_cast<sort_arg<it>*>(args);

            for (int i = 2; i <= arg->arr_len; i *= 2) {
                for (int j = i; j > 1; j /= 2) {
                    // std::cout << "thread=" + std::to_string(arg->rank) + " i=" + std::to_string(i) + " j=" + std::to_string(j) + "\n";
                    std::pair<int, int> range = get_range(arg->threadc,
                                                          arg->arr_len,
                                                          arg->rank,
                                                          j);
                    // std::string a = "thread=" + std::to_string(arg->rank) + " start=" + std::to_string(range.first) + "\n";
                    // std::cout << a;
                    if (range.first < arg->arr_len) {
                        SEQ_TYPE dir = ((range.first / i) % 2) == 1 ? SEQ_TYPE::DECREASE : SEQ_TYPE::INCREASE;
                        std::cout << "executer: rank=" + std::to_string(arg->rank) + " range.first=" + std::to_string(range.first) + " dir=" + std::to_string((int)dir) + "\n";
                        sort_step<comp, it>(arg->first + range.first, arg->first + range.second, j, dir, i);
                    }
                    pthread_barrier_wait(arg->barrier);
                }
            }

            return nullptr;
        }

    } // <-- namespace detail

    template<typename comp, typename it>
    void bitonic_sort(it first, it last, int threadc) {

        using arg_t = detail::sort_arg<it>;
        std::vector<pthread_t> threads(threadc);
        std::vector<arg_t> args;
        args.reserve(threadc);

        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, threadc);

        // first version works only with lengths 2^n and 2^m processor amount
        int arr_len = std::distance(first, last);
        std::cout << "input length = " << arr_len << std::endl;

        for (int i = 0; i < threadc; ++i)
            args.emplace_back(&barrier, first, last, i, threadc, arr_len);

        for (int i = 0; i < threadc; ++i)
            pthread_create(&threads[i], NULL, detail::sort_executor<comp, it>, &args[i]);

        for (auto &&thread : threads)
            pthread_join(thread, NULL);

        pthread_barrier_destroy(&barrier);
    }

} // <-- namespace bitonic_sort

#endif
