#include "Note.hpp"

Note::Note(int midiNum, double noteOn) {
    this->midiNum = midiNum;
    this->noteOn = noteOn;
    active = true;
}
