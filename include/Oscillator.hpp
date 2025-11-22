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
    int semitone = 0;
    int detune = 0;
    double pulseWidth = 0.5;

    bool retrigger;
    float startPhase;

    void run(double elapsed);

    Oscillator();

private:
    double SineWave(double phase);
    double TriangleWave(double phase);
    double SquareWave(double phase, int voice);
    double SawWave(double phase);
    double Noise();

    double getSample(double, int, int);

    double HZtoAV(double);

    enum Inputs {
        FREQUENCY,
        AMPLITUDE,
        PULSE_WIDTH,
        SYNC,
    };
    void initialiseInputs();

    array<int, POLYPHONY + 1> activeVoices{};
    array<double, POLYPHONY + 1> startTimes{};
    // per-voice phase accumulator in [0,1)
    array<double, POLYPHONY + 1> phaseAcc{};
};

REGISTER_COMPONENT(Oscillator);