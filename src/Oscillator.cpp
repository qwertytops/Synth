#include "Oscillator.hpp"
#include "Input.hpp"
#include "Connection.hpp"

void Oscillator::run(double elapsed) {

    while (consumeMidiEvent()) {
        if (currentMidiEvent.type == Event::NOTE_ON) {
            activeVoices[currentMidiEvent.voice] = currentMidiEvent.midiNum;
            startTimes[currentMidiEvent.voice] = elapsed;
        }
    }

    for (int i = 0; i < activeVoices.size(); i++) {
        int midi = activeVoices[i];
        
        if (midi < 0) continue;

        double sample = getSample(elapsed, midi, startTimes[i]);
        
        for (auto& conn : outgoingConnections) {
            conn->destination->add(i, sample);
        } 
    }
}

double Oscillator::getSample(double elapsed, int midi, double startTime) {
    double frequency = 440.0 * pow(2.0, ((midi + 12 * octave) - 69) / 12.0);
    double detuneRatio = pow(2.0, detune / 1200.0);
    frequency = frequency * detuneRatio;

    double phase;

    if (retrigger) {
        phase = (elapsed - startTime) * frequency + startPhase;
    } else {
        phase = startTime * frequency + startPhase;
    }
    phase -= floor(phase);

    double sample = 0.0;
    switch (waveType) {
    case WaveType::SINE:
        sample = SineWave(phase);
        break;
    case WaveType::SQUARE:
        sample = SquareWave(phase);
        break;
    case WaveType::TRI:
        sample = TriangleWave(phase);
        break;
    case WaveType::SAW:
        sample = SawWave(phase);
        break;
    case WaveType::NOISE:
        sample = Noise();
        break;
    default:
        sample = 0;
        break;
    }
    // cout << sample << endl;
    return sample * level;
}

Oscillator::Oscillator() {
    waveType = WaveType::SAW;
    retrigger = true;
    startPhase = 0.5;
    this->octave = 5;
    this->detune = 0;
    initialiseInputs();
    name = "Oscillator";
}

double Oscillator::SineWave(double phase) {
    return sin(HZtoAV(phase));
}

double Oscillator::TriangleWave(double phase) {
    double sine = SineWave(phase);
    return asin(sine) * M_2_PI;
}

double Oscillator::SquareWave(double phase) {
    return phase < 0.5 ? 1.0 : -1.0;
}

// sawtooth down
double Oscillator::SawWave(double phase) {
    return 2 * (1 - phase) - 1;
}

double Oscillator::Noise() {
    return 2 * ((double)rand() / (double)RAND_MAX - 1) - 1;
}

double Oscillator::HZtoAV(double hz) {
    return hz * 6.2831853072;
}

void Oscillator::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
    inputs.push_back(new Input("Frequency", this));
    inputs.push_back(new Input("Amplitude", this));
    inputs.push_back(new Input("Pulse Width", this));
    inputs.push_back(new Input("Sync", this));
}
