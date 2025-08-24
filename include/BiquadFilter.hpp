#pragma once

#include <cmath>
#include <limits>
#include <utility>

#include "SynthComponent.hpp"
#include "SynthComponentFactory.hpp"
#include "FilterMode.hpp"

using namespace std;

class BiquadFilter : public SynthComponent {
public:
    BiquadFilter();
    BiquadFilter(FilterMode mode, double frequency, double Q, double gainDB = 0.0)
        : mode(mode), sampleRate(44100), frequency(frequency), Q(Q), gainDB(gainDB)
    {
        calculateCoefficients();
    }

    void run(double elapsed);
    double applyFilter(double);

    void setFrequency(double frequency);
    void setQ(double Q);
    void setGain(double gainDB);
    void setMode(FilterMode mode);
    void setBypass(bool bypass);

    double getFrequency();
    double getQ();
    double getGain();
    FilterMode getMode();

private:
    bool bypass;
    
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

    enum Inputs {
        MAIN,
        FREQUENCY,
        QUALITY,
        GAIN,
    };
};

REGISTER_COMPONENT(BiquadFilter);