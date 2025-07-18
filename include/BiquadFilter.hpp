#pragma once

#include <cmath>
#include <limits>

#include "FilterMode.hpp"

using namespace std;

class BiquadFilter {
public:
    BiquadFilter(FilterMode mode, double frequency, double Q, double gainDB = 0.0)
        : mode(mode), sampleRate(44100), frequency(frequency), Q(Q), gainDB(gainDB) {
        calculateCoefficients();
    }

    double run(double input);

    void setFrequency(double frequency);
    void setQ(double Q);
    void setGain(double gainDB);
    void setMode(FilterMode mode);

    double getFrequency();
    double getQ();
    double getGain();
    FilterMode getMode();

private:
    FilterMode mode;
    double sampleRate;
    double frequency;
    double Q;
    double gainDB;

    // Coefficients
    double a1 = 0, a2 = 0;
    double b0 = 0, b1 = 0, b2 = 0;

    // State variables (delay elements)
    double x1 = 0, x2 = 0;
    double y1 = 0, y2 = 0;

    void calculateCoefficients();
};
