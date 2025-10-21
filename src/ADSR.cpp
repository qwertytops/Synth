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

    // cout << "running adsr" << endl;
    while (consumeMidiEvent()) {
        if (currentMidiEvent.type == Event::NOTE_ON) {
            activeVoices[currentMidiEvent.voice] = NoteInfo{currentMidiEvent.time, 0, 0};
        } else if (currentMidiEvent.type == Event::NOTE_OFF) {
            activeVoices[currentMidiEvent.voice].noteOff = elapsed;
        }
    }
    Input *mainInput = inputs.at(Inputs::MAIN);
    for (int i = 0; i < POLYPHONY + 1; i++) {
        int voice = i;
        double value = mainInput->values.at(i);

        NoteInfo* voiceLoc = &activeVoices[voice];
        // cout << "voice " << voice << " on: " << voiceLoc->noteOn << endl;

        // cout << value << endl;
        // cout << getAmplitudeMultiplier(elapsed, *voiceLoc) << endl;

        double sample = value * getAmplitudeMultiplier(elapsed, *voiceLoc);

        // update release level
        if (voiceLoc->noteOff < voiceLoc->noteOn) {
            voiceLoc->releaseLevel = sample;
        }

        for (auto& conn : outgoingConnections) {
            // cout << "adding " << sample << " to voice " << voice << endl;
            conn->destination->add(voice, sample);
        }
    }
}

double ADSR::getAmplitudeMultiplier(double elapsed, NoteInfo note) {

    if (note.noteOn == 0) {
        return 0;
    }

    double amplitude = 0;
    double lifetime = elapsed - note.noteOn;


    if (note.noteOff == 0) {
        // cout << "ADS" << endl;
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
        // cout << "R" << endl;
        // R
        double timeSinceRelease = elapsed - note.noteOff;
        if (timeSinceRelease >= release) {
            note.noteOn = 0;
            return 0;
        }
        if (release > 0) { // div by 0 guard may not be necessary after above block
            amplitude = note.releaseLevel * (1.0 - timeSinceRelease / release);
        } else {
            return 0;
        }
    }

    // cout << "returning " << amplitude << endl;
    return amplitude;
}

void ADSR::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
}