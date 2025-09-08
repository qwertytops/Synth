#pragma once

#include <utility>

#include "Synth.hpp"
#include "SynthComponentFactory.hpp"

#define NUM_INPUTS 4

class Mixer : public SynthComponent {
public:
    double level;
    double inLevels[4];

    void run(double elapsed);

    Mixer();

private:
    enum Inputs {
       IN1,
       IN2,
       IN3,
       IN4
    };
    void initialiseInputs();
};

REGISTER_COMPONENT(Mixer);