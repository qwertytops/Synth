#pragma once

#include <utility>

#include "Note.hpp"
#include "Synth.hpp"
#include "SynthComponentFactory.hpp"

using namespace std;

class ADSR : public SynthComponent {
public:
    double attack;
    double decay;
    double sustain;
    double release;

    ADSR();
    ADSR(double a, double d, double s, double r);

    void run(double elapsed);

private:
    double getAmplitude(double elapsed, Note* note);
    enum Inputs {
        MAIN
    };
    void initialiseInputs();
};

REGISTER_COMPONENT(ADSR);