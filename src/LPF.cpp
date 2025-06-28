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

void LPF::setCutoff(int cutoff) {
    alpha = calculateAlpha(cutoff);
    this->cutoff = cutoff;
}

LPF::LPF(double cutoff) : cutoff(cutoff), alpha(calculateAlpha(cutoff)), init(false) {}

double LPF::calculateAlpha(int cutoff) {
    double dt = 1 / 44100.0;
    double RC = 1 / (2 * M_PI * cutoff);
    return dt / (RC + dt);
}
