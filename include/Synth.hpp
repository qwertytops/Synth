#pragma once

#include <vector>
#include <thread>
#include <mutex>

#include "CoreAudioWaveMaker.hpp"
#include "Oscillator.hpp"
#include "ADSR.hpp"
#include "Note.hpp"

using namespace std;

class Synth {
public:
    double volume = 0.2;
    int octave = 4;

    vector<Oscillator*> oscillators;
    vector<ADSR*> envelopes;

    bool keys[18];
    vector<Note> notesBeingPlayed;
    std::mutex mtx;

    double MakeSound(double elapsed);
    void ProcessInput(int octave);

    Synth(vector<Oscillator*> oscs, vector<ADSR*> envs, int octave);

private:
    CoreAudioWaveMaker<float> player;
};