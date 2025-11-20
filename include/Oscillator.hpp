#pragma once
#include <cmath>
#include <utility>

#include "Globals.hpp"
#include "Synth.hpp"
#include "SynthComponentFactory.hpp"

using namespace std;

enum class WaveType {
    SINE,
    TRI,
    SQUARE,
    SAW,
    NOISE
};
inline const vector<WaveType> waveTypes = {
    WaveType::SINE, WaveType::TRI, WaveType::SQUARE, WaveType::SAW, WaveType::NOISE
};
static const string waveTypeToString(WaveType type) {
    switch (type) {
        case WaveType::SINE:   return "Sine";
        case WaveType::TRI:    return "Triangle";
        case WaveType::SQUARE: return "Square";
        case WaveType::SAW:    return "Saw";
        case WaveType::NOISE:  return "NOISE";
        default:               return "UNKNOWN";
    }
}

class Oscillator : public SynthComponent {
public:
    double level = 0.5;
    WaveType waveType;
    int octave;
    int semitone;
    int detune;

    bool retrigger;
    float startPhase;

    void run(double elapsed);

    Oscillator();

private:
    double SineWave(double phase);
    double TriangleWave(double phase);
    double SquareWave(double phase);
    double SawWave(double phase);
    double Noise();

    double getSample(double, int, double);

    double HZtoAV(double);

    enum Inputs {
        MAIN,
        FREQUENCY,
        AMPLITUDE,
        PULSE_WIDTH,
        SYNC,
    };
    void initialiseInputs();

    array<int, POLYPHONY + 1> activeVoices{};
    array<double, POLYPHONY + 1> startTimes{};
};

REGISTER_COMPONENT(Oscillator);