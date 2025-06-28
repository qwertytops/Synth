#pragma once

#include <iostream>
#include "Note.hpp"

using namespace std;

class ADSR {
public:
    double attack;
    double decay;
    double sustain;
    double release;

    // double triggerOn = 0;
    // double triggerOff = 0;

    // bool noteOn = false;

    ADSR();
    ADSR(double a, double d, double s, double r);

    double GetAmplitude(double time, Note* note);
    // void NoteOn(double time);
    // void NoteOff(double time);

private:
    // double offAmplitude;
};