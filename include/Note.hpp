#pragma once

class Note {
public:
    int midiNum;
    bool active;
    double noteOn;
    double noteOff;
    double offAmplitude;

    Note(int midiNum, double noteOn);
};