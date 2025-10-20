#pragma once

class Note {
public:
    int voice;
    int midiNum;
    double noteOn;
    double noteOff;
    double offAmplitude;
    bool finished;
    bool active;

    Note(int midiNum, double noteOn);
};