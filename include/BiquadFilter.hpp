#pragma once

#include <cmath>

using namespace std;

class BiquadFilter {
public:
    enum Mode {
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOTCH,
        PEAK,
        LOWSHELF,
        HIGHSHELF
    };

    BiquadFilter(Mode mode, double sampleRate, double frequency, double Q, double gainDB = 0.0)
        : mode(mode), sampleRate(sampleRate), frequency(frequency), Q(Q), gainDB(gainDB) {
        calculateCoefficients();
    }

    double process(double input);

    void setFrequency(double frequency);
    void setQ(double Q);
    void setGain(double gainDB);
    void setType(Mode mode);

private:
    Mode mode;
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
