#include "BiquadFilter.hpp"

#include <iostream>

double BiquadFilter::run(double input) {
    double output = b0 * input + b1 * x1 + b2 * x2
                    - a1 * y1 - a2 * y2;

    // Shift delay buffers
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;

    cout << output << endl;

    return output;
}

void BiquadFilter::setFrequency(double frequency) {
    this->frequency = frequency;
    calculateCoefficients();
}
void BiquadFilter::setQ(double Q) {
    this->Q = Q;
    calculateCoefficients();
}
void BiquadFilter::setGain(double gainDB) {
    this->gainDB = gainDB;
    calculateCoefficients();
}
void BiquadFilter::setMode(FilterMode mode) {
    this->mode = mode;
    calculateCoefficients();
}

double BiquadFilter::getFrequency() {
    return frequency;
}
double BiquadFilter::getQ() {
    return Q;
}
double BiquadFilter::getGain() {
    return gainDB;
}
FilterMode BiquadFilter::getMode() {
    return mode;
}

void BiquadFilter::calculateCoefficients() {
    // Clamp Q to a safe minimum
    double safeQ = (std::abs(Q) < 1e-8) ? 1e-8 : Q;
    double A = pow(10, gainDB / 40.0);
    if (A < 1e-8) A = 1e-8; // Prevent A from being too small or negative
    double omega = 2 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2 * safeQ);
    double cos_omega = cos(omega);

    switch (mode) {
        case FilterMode::LOWPASS:
            b0 = (1 - cos_omega) / 2;
            b1 = 1 - cos_omega;
            b2 = (1 - cos_omega) / 2;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case FilterMode::HIGHPASS:
            b0 = (1 + cos_omega) / 2;
            b1 = -(1 + cos_omega);
            b2 = (1 + cos_omega) / 2;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case FilterMode::BANDPASS:
            b0 = alpha;
            b1 = 0;
            b2 = -alpha;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case FilterMode::NOTCH:
            b0 = 1;
            b1 = -2 * cos_omega;
            b2 = 1;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case FilterMode::PEAK: {
            b0 = 1 + alpha * A;
            b1 = -2 * cos_omega;
            b2 = 1 - alpha * A;
            double a0 = 1 + alpha / A;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha / A;
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            return;
        }

        case FilterMode::LOWSHELF: {
            double beta = sqrt(A) / safeQ;
            double a0 = (A + 1) - (A - 1) * cos_omega + 2 * beta * sin(omega);
            b0 =    A * ((A + 1) - (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 =  2*A * ((A - 1) - (A + 1) * cos_omega);
            b2 =    A * ((A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 = -2 * ((A - 1) + (A + 1) * cos_omega);
            a2 =     (A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega);
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            return;
        }

        case FilterMode::HIGHSHELF: {
            double beta = sqrt(A) / safeQ;
            double a0 = (A + 1) + (A - 1) * cos_omega + 2 * beta * sin(omega);
            b0 =    A * ((A + 1) + (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 = -2*A * ((A - 1) + (A + 1) * cos_omega);
            b2 =    A * ((A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 =  2 * ((A - 1) - (A + 1) * cos_omega);
            a2 =      (A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega);
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            return;
        }
    }

    // Normalize
    double a0 = 1 + alpha;
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}