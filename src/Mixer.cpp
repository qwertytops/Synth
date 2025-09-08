#include "Mixer.hpp"
#include "NoteInput.hpp"
#include "Connection.hpp"

Mixer::Mixer() {
    for (int i = 0; i < NUM_INPUTS; i++) {
        inLevels[i] = 0.25;
    }
}

void Mixer::run(double elapsed) {
    for (int i = 0; i < NUM_INPUTS; i++) {
        NoteInput* currentInput = inputs.at(i);
        for (int i = 0; i < currentInput->endIndex; i++) {
            auto& pair = currentInput->pairs.at(i);
            Note* note = pair.first;
            double sample = pair.second * inLevels[i];

            for (auto& conn : outgoingConnections) {
                conn->destination->add(make_pair(note, sample));
            }
        }
    }
}

void Mixer::initialiseInputs() {
    inputs.push_back(new NoteInput("IN1", this));
    inputs.push_back(new NoteInput("IN2", this));
    inputs.push_back(new NoteInput("IN3", this));
    inputs.push_back(new NoteInput("IN4", this));
}