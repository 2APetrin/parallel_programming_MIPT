#ifndef INTEGRAL_EVALUATOR_HPP
#define INTEGRAL_EVALUATOR_HPP

#include <functional>

namespace integral_evaluator {

class integrator final {
    std::function<double(double)> f_;
    double a_;
    double b_;
    double acc_;
    int n_workers_;

    double answer;
    bool evaluated;

public:
    integrator(const std::function<double(double)> f, double a,
               double b, double acc, int n_workers) :
               f_(f), a_(a), b_(b), acc_(acc),
               n_workers_(n_workers), answer(0), evaluated(false) {}

private:
    double serial_evaluate(double a, double b, double acc);
    double evaluate();

public:
    double get_answer() {
        if (evaluated)
            return answer;
        if (n_workers_ == 1) {
            answer = serial_evaluate(a_, b_, acc_);
            evaluated = true;
        } else {
            answer = evaluate();
            evaluated = true;
        }
        return answer;
}

};

} // <-- namespace integral_evaluator

#endif