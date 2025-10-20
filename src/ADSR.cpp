#include "ADSR.hpp"
#include "Input.hpp"
#include "Connection.hpp"

ADSR::ADSR() {
    attack = 0.01;
    decay = 0.1;
    sustain = 1.0;
    release = 0.1;
    initialiseInputs();
    name = "ADSR";
}

ADSR::ADSR(double a, double d, double s, double r) {
    attack = a;
    decay = d;
    sustain = s;
    release = r;
    initialiseInputs();
    name = "ADSR";
}

void ADSR::run(double elapsed) {

    while (consumeMidiEvent()) {
        if (currentMidiEvent.type == Event::NOTE_ON) {
            activeVoices[currentMidiEvent.voice] = NoteInfo{currentMidiEvent.time, 0, 0};
        } else if (currentMidiEvent.type == Event::NOTE_OFF) {
            activeVoices[currentMidiEvent.voice].noteOff = elapsed;
        }
    }
    Input *mainInput = inputs.at(Inputs::MAIN);
    for (int i = 0; i < mainInput->endIndex; i++) {
        cout << "here2" << endl;
        cout << mainInput->pairs.size() << " | " << i << endl;
        auto &pair = mainInput->pairs.at(i);
        int voice = pair.first;
        double value = pair.second;

        NoteInfo* voiceLoc = &activeVoices[voice];

        double sample = getAmplitude(elapsed, *voiceLoc);

        // update release level
        if (voiceLoc->noteOff < voiceLoc->noteOn) {
            voiceLoc->releaseLevel = sample;
        }

        for (auto& conn : outgoingConnections) {
            conn->destination->add(make_pair(voice, sample));
        }
    }
}

double ADSR::getAmplitude(double elapsed, NoteInfo note) {
    double amplitude = 0;
    double lifetime = elapsed - note.noteOn;

    if (note.noteOff > 0) {
        if (lifetime < attack) { // A
            amplitude = (lifetime / attack) * 1;
        } else if (lifetime < (attack + decay)) { // D
            amplitude = ((lifetime - attack) / decay) * (sustain - 1) + 1;
        }
        else { // S
            amplitude = sustain;
        }
        note.releaseLevel = amplitude; // bit of a hack
    }
    else {
        // R
        double timeSinceRelease = elapsed - note.noteOff;
        if (timeSinceRelease >= release) {
            return 0;
        }
        if (release > 0) { // div by 0 guard may not be necessary after above block
            amplitude = note.releaseLevel * (1.0 - timeSinceRelease / release);
        } else {
            return 0;
        }
    }

    return amplitude;
}

void ADSR::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
}