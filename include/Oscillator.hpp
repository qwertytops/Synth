#pragma once
#include <cmath>

#include "WaveType.hpp"

using namespace std;

class Oscillator {
public:
    WaveType waveType;
    int octave;
    int detune;

    double run(double elapsed, int noteMidi);

    Oscillator(WaveType w, int octave);
    Oscillator(WaveType w, int octave, int detune);

private:
    double SineWave(double elapsed, double frequency);
    double TriangleWave(double elapsed, double frequency);
    double SquareWave(double elapsed, double frequency);
    double SawWave(double elapsed, double frequency);
    double Saw2Wave(double elapsed, double frequency);
    double Noise();

    double HZtoAV(double);
};