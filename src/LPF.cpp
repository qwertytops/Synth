#include "LPF.hpp"

double LPF::run(double input) {
    if (!init) {
        previous = input;
        init = true;
    }
    double out = previous + alpha * (input - previous);
    previous = out;
    return out;
}

LPF::LPF(double alpha) : alpha(alpha), init(false) {}
