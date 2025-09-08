#include "Mixer.hpp"
#include "Connection.hpp"

Mixer::Mixer()
    : inLevels({ 0.25, 0.25, 0.25, 0.25 }), level(1) {
    initialiseInputs();
    name = "Mixer";
}

void Mixer::run(double elapsed) {
    double result = 0;
    double earliest = 0;
    for (auto& input : inputs) {
        for (auto& pair : input->pairs) {
            Note* note = pair.first;

            earliest = max(note->noteOn, earliest);

            result += pair.second;
        }
    }

    if (earliest != 0) {
        for (auto& conn : outgoingConnections) {
            conn->destination->pairs.push_back(make_pair(new Note(0, earliest), result));
        }
    }
}

void Mixer::initialiseInputs() {
    inputs.push_back(new NoteInput("In1", this));
    inputs.push_back(new NoteInput("In2", this));
    inputs.push_back(new NoteInput("In3", this));
    inputs.push_back(new NoteInput("In4", this));
}