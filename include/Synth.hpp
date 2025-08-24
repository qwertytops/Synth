#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <mutex>
#include <set>

#include "CoreAudioWaveMaker.hpp"
#include "Oscillator.hpp"
#include "ADSR.hpp"
#include "BiquadFilter.hpp"
#include "Note.hpp"
#include "SynthComponent.hpp"

using namespace std;

class Synth {
public:
    double volume = 0.5;
    int octave = 4;

    vector<SynthComponent*> components;

    bool keys[18];
    vector<Note> notesBeingPlayed;
    std::mutex mtx;

    double MakeSound(double elapsed);
    void ProcessInput(int octave);

    Synth(vector<SynthComponent*> components, int octave);

private:
    CoreAudioWaveMaker<float> player;

    int inputs = 0;
    vector<SynthComponent*> establishProcessingOrder();
    vector<SynthComponent*> processingOrder;
};