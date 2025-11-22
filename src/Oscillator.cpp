#include "Oscillator.hpp"
#include "Input.hpp"
#include "Connection.hpp"
#include <cmath>
#include <cstdlib>

void Oscillator::run(double elapsed) {

    while (consumeMidiEvent()) {
        if (currentMidiEvent.type == Event::NOTE_ON) {
            activeVoices[currentMidiEvent.voice] = currentMidiEvent.midiNum;
            startTimes[currentMidiEvent.voice] = elapsed;

            if (retrigger) {
                phaseAcc[currentMidiEvent.voice] = startPhase - floor(startPhase);
            }
        }
    }

    for (int i = 0; i < POLYPHONY; i++) {
        int midi = activeVoices[i];
        
        if (midi < 0) continue;

        double sample = getSample(elapsed, midi, i);
        
        for (auto& conn : outgoingConnections) {
            conn->destination->add(i, sample);
        } 
    }
}

double Oscillator::getSample(double elapsed, int midi, int voice) {
    // arbitrary choice of 1.0 = 2 octaves up
    double modulation = (
        inputs.at(Inputs::FREQUENCY)->values.at(voice)
        + inputs.at(Inputs::FREQUENCY)->values.at(MONO_VOICE)
    );
    // cout << modulation << endl;

    double frequency = 440.0 * pow(2.0, ((midi + semitone + modulation + 12.0 * octave) - 69.0) / 12.0);
    double detuneRatio = pow(2.0, detune / 1200.0);
    frequency *= detuneRatio;

    double phase;

    // if (retrigger) {
    //     phase = (elapsed - startTimes.at(voice)) * frequency + startPhase;
    // } else {
    //     phase = startTimes.at(voice) * frequency + startPhase;
    // }
    // phase -= floor(phase);
    
    // advance phase accumulator by frequency / sampleRate
    double phaseInc = frequency / SAMPLE_RATE;
    phaseAcc[voice] += phaseInc;
    phaseAcc[voice] -= floor(phaseAcc[voice]);

    phase = phaseAcc[voice];

    double sample = 0.0;
    switch (waveType) {
    case WaveType::SINE:
        sample = SineWave(phase);
        break;
    case WaveType::SQUARE:
        sample = SquareWave(phase, voice);
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

    return sample * (level
        + inputs.at(Inputs::AMPLITUDE)->values.at(voice)
        + inputs.at(Inputs::AMPLITUDE)->values.at(MONO_VOICE)
    );
}

Oscillator::Oscillator() {
    waveType = WaveType::SAW;
    retrigger = true;
    startPhase = 0.5;
    this->octave = 5;
    initialiseInputs();
    name = "Oscillator";
}

double Oscillator::SineWave(double phase) {
    return sin(HZtoAV(phase));
}

double Oscillator::TriangleWave(double phase) {
    double sine = SineWave(phase);
    return asin(sine) * M_2_PI;

    // if (phase < 0.5) {
    //     return -1.0 + 4.0 * phase;
    // } else {
    //     return 3.0 - 4.0 * phase;
    // }
}

double Oscillator::SquareWave(double phase, int voice) {
    double pw = pulseWidth
        + inputs.at(Inputs::PULSE_WIDTH)->values.at(voice)
        + inputs.at(Inputs::PULSE_WIDTH)->values.at(MONO_VOICE);

    return phase < pw ? 1.0 : -1.0;
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
    inputs.push_back(new Input("Frequency", this));
    inputs.push_back(new Input("Amplitude", this));
    inputs.push_back(new Input("Pulse Width", this));
    inputs.push_back(new Input("Sync", this));
}
