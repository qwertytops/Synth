#pragma once

#include <cmath>
#include <limits>
#include <utility>

#include "Synth.hpp"
#include "SynthComponentFactory.hpp"
#include "FilterMode.hpp"

using namespace std;

struct BiquadParams {
    double frequency;
    double Q;
    double gainDB;
    FilterMode mode;
    bool bypass;
    // precomputed coefficients
    double b0, b1, b2, a1, a2;
};

class BiquadFilter : public SynthComponent {
public:
    BiquadFilter();
    BiquadFilter(FilterMode mode, double frequency, double Q, double gainDB = 0.0)
        : mode(mode), frequency(frequency), Q(Q), gainDB(gainDB)
    {
        calculateCoefficients();
        // this is out of date but also out of use rn
    }

    void run(double elapsed);
    double applyFilter(double, int);

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
    double frequency;
    double Q;
    double gainDB;

    // Coefficients
    double a1 = 0, a2 = 0;
    double b0 = 0, b1 = 0, b2 = 0;

    // State variables (delay elements)
    array<double, POLYPHONY + 1> x1{}, x2{}, y1{}, y2{};

    void calculateCoefficients();

    enum Inputs {
        MAIN,
        FREQUENCY,
        QUALITY,
        GAIN,
    };
    void initialiseInputs();

    array<int, POLYPHONY + 1> activeVoices{};
};

REGISTER_COMPONENT(BiquadFilter);