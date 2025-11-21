#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <mutex>
#include <set>
#include <atomic>
#include <array>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>

#include "CoreAudioWaveMaker.hpp"
#include "Oscillator.hpp"
#include "ADSR.hpp"
#include "BiquadFilter.hpp"
#include "Note.hpp"
#include "Input.hpp"
#include "SynthComponent.hpp"
#include "VoiceAllocator.hpp"

using namespace std;
using namespace boost::lockfree;

class Synth {
public:
    double volume = 0.5;
    int octave = 4;

    vector<SynthComponent*> components;

    // atomic key state updated by input thread
    std::array<std::atomic_bool, 18> keys;

    void ProcessInput(int octave);

    // New: block render API used by audio device
    void RenderAudioBlock(float* outBuffer, UInt32 frames);

    double MakeSound(double elapsed);

    Input* mainOut;

    Synth(int octave);

    void addComponent(SynthComponent*);
    void removeComponent(SynthComponent*);

    vector<Input*> allInputs;
    void getAllInputs();

    void establishProcessingOrder();

private:
    CoreAudioWaveMaker<float> player;
    VoiceAllocator voiceAllocator;

    int inputs = 0;
    vector<SynthComponent*> processingOrder;

    spsc_queue<Event> eventBuffer;

    void broadcastMidiEvent(Event e);
};