#include "Mixer.hpp"
#include "Input.hpp"
#include "Connection.hpp"

Mixer::Mixer() {
    for (int i = 0; i < NUM_INPUTS; i++) {
        inLevels[i] = 0.25;
    }
    initialiseInputs();
    name = "Mixer";
}

void Mixer::run(double elapsed) {
    for (int i = 0; i < NUM_INPUTS; i++) {
        Input* currentInput = inputs.at(i);
        for (int i = 0; i < POLYPHONY + 1; i++) {
            int voice = i;
            double sample = currentInput->values.at(i) * inLevels[i];

            for (auto& conn : outgoingConnections) {
                conn->destination->add(voice, sample);
            }
        }
    }
}

void Mixer::initialiseInputs() {
    inputs.push_back(new Input("IN1", this));
    inputs.push_back(new Input("IN2", this));
    inputs.push_back(new Input("IN3", this));
    inputs.push_back(new Input("IN4", this));
}