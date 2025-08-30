#include "Oscillator.hpp"
#include "Input.hpp"
#include "Connection.hpp"

void Oscillator::run(double elapsed) {
    for (auto& pair : inputs.at(Inputs::MAIN)->pairs) {
        Note* note = pair.first;
        double sample = getSample(elapsed, note);

        for (auto& conn : outgoingConnections) {
            conn->destination->pairs.push_back(make_pair(note, sample));
        }
    }
}

double Oscillator::getSample(double elapsed, Note* note) {
    double frequency = 440.0 * pow(2.0, ((note->midiNum + 12 * octave) - 69) / 12.0);
    double detuneRatio = pow(2.0, detune / 1200.0);
    frequency = frequency * detuneRatio;

    double sample = 0.0;
    switch (waveType) {
    case WaveType::SINE:
        sample = SineWave(elapsed, frequency);
        break;
    case WaveType::SQUARE:
        sample = SquareWave(elapsed, frequency);
        break;
    case WaveType::TRI:
        sample = TriangleWave(elapsed, frequency);
        break;
    case WaveType::SAW:
        sample = SawWave(elapsed, frequency);
        break;
    case WaveType::SAW2:
        sample = Saw2Wave(elapsed, frequency);
        break;
    case WaveType::NOISE:
        sample = frequency ? Noise() : 0;
        break;
    default:
        sample = 0;
        break;
    }
    return sample * level;
}

Oscillator::Oscillator() {
    waveType = WaveType::SINE;
    this->octave = 4;
    this->detune = 0;
    initialiseInputs();
    name = "Oscillator";
}

Oscillator::Oscillator(WaveType w, int octave) {
    waveType = w;
    this->octave = octave;
    this->detune = 0;
    initialiseInputs();
    name = "Oscillator";
}
Oscillator::Oscillator(WaveType w, int octave, int detune) {
    waveType = w;
    this->octave = octave;
    this->detune = detune;
    initialiseInputs();
    name = "Oscillator";
}

double Oscillator::SineWave(double elapsed, double frequency) {
    return sin(HZtoAV(frequency) * elapsed);
}

double Oscillator::TriangleWave(double elapsed, double frequency) {
    double sine = SineWave(elapsed, frequency);
    return HZtoAV(asin(sine));
}

double Oscillator::SquareWave(double elapsed, double frequency) {
    double sine = SineWave(elapsed, frequency);
    return sine > 0 ? 1.0 : -1.0;
}

double Oscillator::SawWave(double elapsed, double frequency) {
    return 2.0 * fmod(elapsed * frequency, 1.0) - 1.0;
}

double Oscillator::Saw2Wave(double elapsed, double frequency) {
    double dOutput = 0.0;

    for (double n = 1.0; n < 40.0; n++)
        dOutput += (sin(n * HZtoAV(frequency) * elapsed)) / n;

    return dOutput * (2.0 / M_PI);
}

double Oscillator::Noise() {
    return 2 * ((double)rand() / (double)RAND_MAX - 1);
}

double Oscillator::HZtoAV(double hz) {
    return hz * 2 * M_PI;
}

void Oscillator::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
    inputs.push_back(new Input("Frequency", this));
    inputs.push_back(new Input("Amplitude", this));
    inputs.push_back(new Input("Pulse Width", this));
    inputs.push_back(new Input("Sync", this));
}
