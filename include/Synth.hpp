#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <mutex>
#include <set>
#include <atomic>
#include <array>
#include <cstddef>
#include <memory>

#include "CoreAudioWaveMaker.hpp"
#include "Oscillator.hpp"
#include "ADSR.hpp"
#include "BiquadFilter.hpp"
#include "Note.hpp"
#include "NoteInput.hpp"
#include "SynthComponent.hpp"

using namespace std;

class Synth {
public:
    double volume = 0.5;
    int octave = 4;

    vector<SynthComponent*> components;

    // atomic key state updated by input thread
    std::array<std::atomic_bool, 18> keys;

    vector<Note*> notesBeingPlayed;
    std::mutex mtx;

    double MakeSound(double elapsed);
    void ProcessInput(int octave);

    // New: block render API used by audio device
    void RenderAudioBlock(float* outBuffer, UInt32 frames);

    // Internal: non-locking version of MakeSound used when caller holds lock
    double MakeSoundLocked(double elapsed);

    NoteInput* mainOut;

    Synth(int octave);

    void addComponent(SynthComponent*);

    vector<NoteInput*> allInputs;
    void getAllInputs();

    void establishProcessingOrder();

private:
    CoreAudioWaveMaker<float> player;

    int inputs = 0;
    vector<SynthComponent*> processingOrder;

    // Lock-free SPSC event queue for note on/off events
    struct Event {
        enum Type : uint8_t { NOTE_ON = 1, NOTE_OFF = 2 } type;
        uint8_t midi;    // midi number (0..127)
        double time;     // time stamp (player time)
    };

    static constexpr size_t EVENT_CAP = 1024; // power of two (adjust if needed)
    std::vector<Event> eventBuffer;           // ring buffer storage
    std::atomic<size_t> evHead{0};            // consumer index
    std::atomic<size_t> evTail{0};            // producer index
    size_t evMask = EVENT_CAP - 1;

    // push/pop helpers (SPSC, non-blocking)
    bool pushEvent(const Event& e);
    bool popEvent(Event& out);
};