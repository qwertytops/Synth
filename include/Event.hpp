#pragma once

#include <cstdint>

struct Event {
    double time;
    int voice;
    enum Type : uint8_t { NOTE_ON = 1, NOTE_OFF = 2 } type;
    uint8_t midiNum;
};