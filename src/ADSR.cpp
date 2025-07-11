#include "ADSR.hpp"

ADSR::ADSR() {
    attack = 0.01;
    decay = 0.1;
    sustain = 1.0;
    release = 0.1;
}

ADSR::ADSR(double a, double d, double s, double r) {
    attack = a;
    decay = d;
    sustain = s;
    release = r;
}

double ADSR::GetAmplitude(double time, Note* note) {
    double amplitude = 0;
    double lifetime = time - note->noteOn;

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
        double timeSinceRelease = time - note->noteOff;
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

// void ADSR::NoteOn(double time) {
//     triggerOn = time;
//     noteOn = true;
// }

// void ADSR::NoteOff(double time, ) {
//     offAmplitude = GetAmplitude(time);
//     triggerOff = time;
//     noteOn = false;
// }