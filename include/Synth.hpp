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
#include "Input.hpp"
#include "SynthComponent.hpp"

using namespace std;

class Synth {
public:
    double volume = 0.5;
    int octave = 4;

    vector<SynthComponent*> components;

    bool keys[18];
    vector<Note*> notesBeingPlayed;
    std::mutex mtx;

    double MakeSound(double elapsed);
    void ProcessInput(int octave);

    // New: block render API used by audio device
    void RenderAudioBlock(float* outBuffer, UInt32 frames);

    // Internal: non-locking version of MakeSound used when caller holds lock
    double MakeSoundLocked(double elapsed);

    Input* mainOut;

    Synth(int octave);

    void addComponent(SynthComponent*);

    vector<Input*> allInputs;
    void getAllInputs();

    void establishProcessingOrder();

private:
    CoreAudioWaveMaker<float> player;

    int inputs = 0;
    vector<SynthComponent*> processingOrder;
};