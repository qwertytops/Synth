#pragma once

#include <utility>

#include "Globals.hpp"
#include "Synth.hpp"
#include "SynthComponentFactory.hpp"

using namespace std;

struct NoteInfo {
    double noteOn;
    double noteOff;
    double releaseLevel;
};

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
    double getAmplitudeMultiplier(double elapsed, NoteInfo* note);
    enum Inputs {
        MAIN
    };
    void initialiseInputs();

    array<NoteInfo, POLYPHONY + 1> activeVoices{};
};

REGISTER_COMPONENT(ADSR);