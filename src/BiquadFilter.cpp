#include "BiquadFilter.hpp"
#include "Input.hpp"
#include "Connection.hpp"

// #include <iostream>

BiquadFilter::BiquadFilter() {
    this->mode = FilterMode::LOWPASS;
    this->frequency = 10000;
    this->Q = 1;
    this->gainDB = 0.0;

    calculateCoefficients();
    initialiseInputs();
    name = "BiquadFilter";
}

void BiquadFilter::run(double elapsed) {
    Input* mainInput = inputs.at(Inputs::MAIN);
    for (int i = 0; i < mainInput->endIndex; i++) {
        auto &pair = mainInput->pairs.at(i);
        int voice = pair.first;
        double value = pair.second;

        value = applyFilter(value, voice);
        for (auto& conn : outgoingConnections) {
            conn->destination->add(make_pair(voice, value));
        }
    }
}

double BiquadFilter::applyFilter(double input, int voice) {
    if (bypass)
        return input;

    double output = b0 * input + b1 * x1[voice] + b2 * x2[voice] - a1 * y1[voice] - a2 * y2[voice];

    // Shift delay buffers
    x2[voice] = x1[voice];
    x1[voice] = input;
    y2[voice] = y1[voice];
    y1[voice] = output;

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

void BiquadFilter::setBypass(bool bypass) {
    this->bypass = bypass;
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
    double omega = 2 * M_PI * frequency / SAMPLE_RATE;
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
            break;
        }

        case FilterMode::LOWSHELF: {
            double beta = sqrt(A) / safeQ;
            double a0 = (A + 1) - (A - 1) * cos_omega + 2 * beta * sin(omega);
            b0 =    A * ((A + 1) - (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 =  2*A * ((A - 1) - (A + 1) * cos_omega);
            b2 =    A * ((A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 = -2 * ((A - 1) + (A + 1) * cos_omega);
            a2 =     (A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega);
            break;
        }

        case FilterMode::HIGHSHELF: {
            double beta = sqrt(A) / safeQ;
            double a0 = (A + 1) + (A - 1) * cos_omega + 2 * beta * sin(omega);
            b0 =    A * ((A + 1) + (A - 1) * cos_omega + 2 * beta * sin(omega));
            b1 = -2*A * ((A - 1) + (A + 1) * cos_omega);
            b2 =    A * ((A + 1) + (A - 1) * cos_omega - 2 * beta * sin(omega));
            a1 =  2 * ((A - 1) - (A + 1) * cos_omega);
            a2 =      (A + 1) - (A - 1) * cos_omega - 2 * beta * sin(omega);
            break;
        }
    }

    // Normalize
    double inv_a0 = 1 / (1 + alpha);
    b0 *= inv_a0;
    b1 *= inv_a0;
    b2 *= inv_a0;
    a1 *= inv_a0;
    a2 *= inv_a0;
}

void BiquadFilter::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
    inputs.push_back(new Input("Frequency", this));
    inputs.push_back(new Input("Quality", this));
    inputs.push_back(new Input("Gain", this));  
}