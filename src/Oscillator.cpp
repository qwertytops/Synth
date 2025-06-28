#include "Oscillator.hpp"

double Oscillator::run(double elapsed, int noteMidi) {
    double frequency = 440.0 * pow(2.0, ((noteMidi + 12 * octave) - 69) / 12.0);
    double detuneRatio = pow(2.0, detune / 1200.0);
    frequency = frequency * detuneRatio;

    switch (waveType) {
    case WaveType::SINE:
        return SineWave(elapsed, frequency);
    case WaveType::SQUARE:
        return SquareWave(elapsed, frequency);
    case WaveType::TRI:
        return TriangleWave(elapsed, frequency);
    case WaveType::SAW:
        return SawWave(elapsed, frequency);
    case WaveType::SAW2:
        return Saw2Wave(elapsed, frequency);
    case WaveType::NOISE:
        return frequency ? Noise() : 0;
    default:
        return 0;
    }
}

Oscillator::Oscillator(WaveType w, int octave) {
    waveType = w;
    this->octave = octave;
    this->detune = 0;
}
Oscillator::Oscillator(WaveType w, int octave, int detune) {
    waveType = w;
    this->octave = octave;
    this->detune = detune;
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

double Oscillator::HZtoAV(double hz)
{
    return hz * 2 * M_PI;
}
