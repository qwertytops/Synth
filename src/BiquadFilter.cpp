#include "BiquadFilter.hpp"

double BiquadFilter::process(double input) {
    double output = b0 * input + b1 * x1 + b2 * x2
                    - a1 * y1 - a2 * y2;

    // Shift delay buffers
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;

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
void BiquadFilter::setType(Mode mode) {
    this->mode = mode;
    calculateCoefficients();
}

void BiquadFilter::calculateCoefficients() {
    double A = pow(10, gainDB / 40); // for peaking and shelving
    double omega = 2 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2 * Q);
    double cos_omega = cos(omega);

    switch (mode) {
        case LOWPASS:
            b0 = (1 - cos_omega) / 2;
            b1 = 1 - cos_omega;
            b2 = (1 - cos_omega) / 2;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case HIGHPASS:
            b0 = (1 + cos_omega) / 2;
            b1 = -(1 + cos_omega);
            b2 = (1 + cos_omega) / 2;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case BANDPASS:
            b0 = alpha;
            b1 = 0;
            b2 = -alpha;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case NOTCH:
            b0 = 1;
            b1 = -2 * cos_omega;
            b2 = 1;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha;
            break;

        case PEAK: {
            b0 = 1 + alpha * A;
            b1 = -2 * cos_omega;
            b2 = 1 - alpha * A;
            a1 = -2 * cos_omega;
            a2 = 1 - alpha / A;
            break;
        }

        case LOWSHELF: {
            double beta = sqrt(A) / Q;
            b0 = A * ((A + 1) - (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 = 2 * A * ((A - 1) - (A + 1) * cos_omega);
            b2 = A * ((A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 = -2 * ((A - 1) + (A + 1) * cos_omega);
            a2 = ((A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega));
            break;
        }

        case HIGHSHELF: {
            double beta = sqrt(A) / Q;
            b0 = A * ((A + 1) + (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 = -2 * A * ((A - 1) + (A + 1) * cos_omega);
            b2 = A * ((A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 = 2 * ((A - 1) - (A + 1) * cos_omega);
            a2 = ((A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega));
            break;
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