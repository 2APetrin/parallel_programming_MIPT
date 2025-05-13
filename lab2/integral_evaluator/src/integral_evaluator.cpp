#include "integral_evaluator.hpp"

#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>

#include <functional>
#include <stack>

using range = std::pair<double, double>;

using namespace integral_evaluator;

constexpr int n_from_stack = 3;
constexpr int max_n_local  = 10;

struct arg_t final {
    std::function<double(double)> f_;
    double a_;
    double b_;
    double acc_;
    int    rank_;
    int    n_workers_;

    int   *n_active_;
    double            *answer_;
    std::stack<range> *global_stack_;

    sem_t *sem_gstk_;
    sem_t *sem_sum_;
    sem_t *sem_task_present_;

public:
    arg_t(std::function<double(double)> f, double a, double b, double acc,
          int rank, int n_workers, int *n_active, double *answer,
          std::stack<range> *global_stack, sem_t *sem_gstk, sem_t *sem_sum,
          sem_t *sem_task_present) :
          f_(f), a_(a), b_(b), acc_(acc), rank_(rank), n_workers_(n_workers),
          n_active_(n_active), answer_(answer), global_stack_(global_stack),
          sem_gstk_(sem_gstk), sem_sum_(sem_sum), sem_task_present_(sem_task_present) {}
};

bool get_tasks(int n, arg_t *arg, std::stack<range> &stk) {
    sem_wait(arg->sem_gstk_);
    --(*arg->n_active_);
    if ((*arg->n_active_ < 1) && arg->global_stack_->empty()) {
        sem_post(arg->sem_task_present_);
        sem_post(arg->sem_gstk_);
        return true;
    }
    sem_post(arg->sem_gstk_);

    sem_wait(arg->sem_task_present_);
    sem_wait(arg->sem_gstk_);

    if ((*arg->n_active_ < 1) && arg->global_stack_->empty()) {
        sem_post(arg->sem_task_present_);
        sem_post(arg->sem_gstk_);
        return true;
    }

    for (int i = 0; i < n && !arg->global_stack_->empty(); ++i) {
        stk.push(arg->global_stack_->top());
        arg->global_stack_->pop();
    }

    if (!arg->global_stack_->empty())
        sem_post(arg->sem_task_present_);

    ++(*arg->n_active_);
    sem_post(arg->sem_gstk_);
    return false;
}

void put_tasks(int n, arg_t *arg, std::stack<range> &stk) {
    sem_wait(arg->sem_gstk_);

    if (arg->global_stack_->empty())
        sem_post(arg->sem_task_present_);

    while (stk.size() > (size_t)n) {
        arg->global_stack_->push(stk.top());
        stk.pop();
    }

    sem_post(arg->sem_gstk_);
}

double integrate_range(range rg, arg_t *arg, std::stack<range> &stk) {
    double sum = 0;

    double a  = rg.first,
           b  = rg.second,
           c  = (a + b) / 2;

    double cv = arg->f_(c),
           av = arg->f_(a),
           bv = arg->f_(b);

    double s_left  = (c - a) * (av + cv) / 2,
           s_right = (b - c) * (bv + cv) / 2,
           s_all   = (b - a) * (bv + av) / 2;

    while (1) {
        double diff = s_all - s_left - s_right;
        double rel_diff = std::abs(diff) / std::abs(s_all);

        if (rel_diff > arg->acc_) {
            stk.emplace(a, c);

            a  = c;
            c  = (a + b) / 2;

            av = cv;
            cv = arg->f_(c);

            s_all   = s_right;
            s_left  = (c - a) * (av + cv) / 2;
            s_right = (b - c) * (bv + cv) / 2;
        } else {
            sum = s_left + s_right;
            break;
        }
    }

    return sum;
}

void* worker(void *args) {
    arg_t *arg = static_cast<arg_t*>(args);
    std::stack<range> stk;

    double local_sum = 0;

    get_tasks(1, arg, stk);
    while(1) {
        if (stk.empty())
            if (get_tasks(n_from_stack, arg, stk)) break;

        auto rng = stk.top(); stk.pop();
        local_sum += integrate_range(rng, arg, stk);

        if (stk.size() > max_n_local)
            put_tasks(n_from_stack, arg, stk);
    }

    sem_wait(arg->sem_sum_);
    *arg->answer_ += local_sum;
    sem_post(arg->sem_sum_);

    return NULL;
}

double integrator::evaluate() {
    std::vector<pthread_t> threads(n_workers_);
    std::vector<arg_t> args;

    std::stack<range> global_stack;
    int N = 10;
    for (int i = 0; i < N; ++i) {
        double step = (b_ - a_) / N;
        double beg = a_ + i * step;
        global_stack.emplace(beg, beg + step);
    }

    sem_t sem_gstk;
    sem_t sem_sum;
    sem_t sem_task_present;

    sem_init(&sem_gstk, 1, 1);
    sem_init(&sem_sum, 1, 1);
    sem_init(&sem_task_present, 1, 1);

    int n_active = n_workers_;

    for (int i = 0; i < n_workers_; ++i)
        args.emplace_back(f_, a_, b_, acc_, i, n_workers_, &n_active, &answer,
                          &global_stack, &sem_gstk, &sem_sum, &sem_task_present);

    for (int i = 0; i < n_workers_; ++i)
        pthread_create(&threads[i], NULL, worker, &args[i]);

    for (auto &&thread : threads)
        pthread_join(thread, NULL);

    sem_destroy(&sem_gstk);
    sem_destroy(&sem_sum);
    sem_destroy(&sem_task_present);

    return answer;
}

double serial_intergate_range(range rg, std::stack<range> &stk, std::function<double(double)> f_, double acc) {
    double sum = 0;

    double a  = rg.first,
           b  = rg.second,
           c  = (a + b) / 2;

    double cv = f_(c),
           av = f_(a),
           bv = f_(b);

    double s_left  = (c - a) * (av + cv) / 2,
           s_right = (b - c) * (bv + cv) / 2,
           s_all   = (b - a) * (bv + av) / 2;

    while (1) {
        double diff = s_all - s_left - s_right;
        double rel_diff = std::abs(diff) / std::abs(s_all);

        if (rel_diff > acc) {
            stk.emplace(a, c);

            a  = c;
            c  = (a + b) / 2;

            av = cv;
            cv = f_(c);

            s_all   = s_right;
            s_left  = (c - a) * (av + cv) / 2;
            s_right = (b - c) * (bv + cv) / 2;
        } else {
            sum = s_left + s_right;
            break;
        }
    }

    return sum;
}

double integrator::serial_evaluate(double a, double b, double acc) {
    double sum = 0;
    std::stack<range> stk;

    int N = 10;
    for (int i = 0; i < N; ++i) {
        double step = (b_ - a_) / N;
        double beg = a_ + i * step;
        stk.emplace(beg, beg + step);
    }

    while (true) {
        if (stk.empty()) break;

        auto rng = stk.top(); stk.pop();
        sum += serial_intergate_range(rng, stk, f_, acc);
    }

    return sum;
}
