#pragma once
#include <cmath>
#include <utility>

#include "Synth.hpp"
#include "SynthComponentFactory.hpp"
#include "WaveType.hpp"

using namespace std;

class Oscillator : public SynthComponent {
public:
    double level = 0.5;
    WaveType waveType;
    int octave;
    int detune;

    void run(double elapsed);
    

    Oscillator();
    Oscillator(WaveType w, int octave);
    Oscillator(WaveType w, int octave, int detune);

private:
    double SineWave(double elapsed, double frequency);
    double TriangleWave(double elapsed, double frequency);
    double SquareWave(double elapsed, double frequency);
    double SawWave(double elapsed, double frequency);
    double Saw2Wave(double elapsed, double frequency);
    double Noise();

    double getSample(double, Note*);

    double HZtoAV(double);

    enum Inputs {
        MAIN,
        FREQUENCY,
        AMPLITUDE,
        PULSE_WIDTH,
        SYNC,
    };
    void initialiseInputs();
};

REGISTER_COMPONENT(Oscillator);