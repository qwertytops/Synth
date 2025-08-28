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
    for (auto& pair : inputs.at(Inputs::MAIN)->pairs) {
        Note* note = pair.first;
        double value = pair.second;

        value *= getAmplitude(elapsed, note);
        for (auto& conn : outgoingConnections) {
            conn->destination->pairs.push_back(make_pair(note, value));
        }
    }
}

double ADSR::getAmplitude(double elapsed, Note* note) {
    double amplitude = 0;
    double lifetime = elapsed - note->noteOn;

    if (note->active) {
        if (lifetime < attack) { // A
            amplitude = (lifetime / attack) * 1;
        } else if (lifetime < (attack + decay)) { // D
            amplitude = ((lifetime - attack) / decay) * (sustain - 1) + 1;
        }
        else { // S
            amplitude = sustain;
        }
        note->offAmplitude = amplitude; // bit of a hack
    }
    else {
        // R
        double timeSinceRelease = elapsed - note->noteOff;
        if (release > 0) {
            amplitude = note->offAmplitude * (1.0 - timeSinceRelease / release);
        } else {
            return 0;
        }
    }

    if (amplitude <= 0.0001) {
        amplitude = 0;
    }

    // cout << "env: " << amplitude << endl;

    return amplitude;
}

void ADSR::initialiseInputs() {
    inputs.push_back(new Input("Main", this));
}