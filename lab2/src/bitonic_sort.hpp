#ifndef BITONIC_SORT_HPP
#define BITONIC_SORT_HPP

#include <vector>
#include <iterator>

// I dunno how to move it into namespace
#include <pthread.h>

namespace bitonic_sort {

namespace detail {

    enum class direction {
        UP,
        DOWN
    };

    template<typename it>
    struct sort_arg {
        pthread_barrier_t *barrier_;
        it first_;
        it last_;
        int thread_rank_;
        int thread_count_;
        int arr_len_;

    public:
        sort_arg(pthread_barrier_t *barrier, it first, it last,
                int thread_rank, int thread_count, int arr_len) :
            barrier_(barrier),
            first_(first),
            last_(last),
            thread_rank_(thread_rank),
            thread_count_(thread_count),
            arr_len_(arr_len) {}
    };

    template<typename it>
    std::pair<int, int> get_range(sort_arg<it> *arg, int block_width) {
        int elems_per_thread = arg->arr_len_ / arg->thread_count_;
        int width = std::max(block_width, elems_per_thread);
        int beg = arg->thread_rank_ * width;
        int end = beg + width;
        return {beg, end};
    }

    inline direction get_range_start_dir(int range_start, int block_width) {
        int blocks_in_range = range_start / block_width;
        return (blocks_in_range % 2) == 0 ? direction::UP : direction::DOWN;
    }

    inline direction swap_direction(direction dir) {
        return (direction::UP == dir) ? direction::DOWN : direction::UP;
    }

    template<typename comp, typename it>
    void sort_step(it curr, it half, it sort_end, direction dir) {
        while(half != sort_end) {
            bool cond_comp = comp()(*curr, *half);
            bool cond_dir  = (dir == direction::DOWN);
            bool cond = (cond_comp == cond_dir);
            if (cond) std::iter_swap(curr, half);
            ++curr; ++half;
        }
    }

    template<typename comp, typename it>
    void thread_sort_step(it first, it last, int sort_width, int block_width, direction start_dir) {
        direction dir = start_dir;
        it curr = first;
        int pos = 0;
        while (curr != last) {
            it sort_end = curr + sort_width;
            it half     = curr + (sort_width / 2);
            pos += sort_width;
            sort_step<comp, it>(curr, half, sort_end, dir);
            curr = sort_end;
            if (pos >= block_width) {
                dir = swap_direction(dir);
                pos = 0;
            }
        }
    }

    template<typename comp, typename it>
    void sort_block(sort_arg<it> *arg, int block_width) {
        auto range = get_range(arg, block_width);
        for (int sort_width = block_width; sort_width > 1; sort_width /= 2) {
            bool working_thread = range.first < arg->arr_len_;
            if (working_thread) {
                direction start_dir = get_range_start_dir(range.first, block_width);
                it first = arg->first_ + range.first;
                it last  = arg->first_ + range.second;
                thread_sort_step<comp, it>(first, last, sort_width, block_width, start_dir);
            }
            pthread_barrier_wait(arg->barrier_);
        }
    }

    template<typename comp, typename it>
    void* sort_executor(void *args) {
        sort_arg<it>* arg = static_cast<sort_arg<it>*>(args);

        for (int block_width = 2; block_width <= arg->arr_len_; block_width *= 2)
            sort_block<comp, it>(arg, block_width);

        return NULL;
    }

} // <-- namespace detail

    // first version works only with lengths 2^n and 2^m processor amount
    template<typename comp, typename it>
    void bitonic_sort(it first, it last, int threadc, comp) {
        using arg_t = detail::sort_arg<it>;
        std::vector<pthread_t> threads(threadc);
        std::vector<arg_t> args;
        args.reserve(threadc);

        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, threadc);

        int arr_len = std::distance(first, last);

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
