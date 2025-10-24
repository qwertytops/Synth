#pragma once
#include <array>
#include <iostream>
#include "Globals.hpp"

#undef slots

struct VoiceAllocator {

    struct Slot {
        bool active = false;
        uint8_t note = 0;
        double startTime = 0.0;
    };

    std::array<Slot, POLYPHONY + 1> slots;

    // Uses oldest free voice or oldest voice if none is free
    int allocate(uint8_t note, double time) {
        int oldest = 0;
        bool oldestIsFree = !slots[0].active;
        double oldestTime = slots[0].startTime;
        for (int i = 1; i < POLYPHONY; i++) {
            if (slots[i].startTime < oldestTime) {
                if (!slots[i].active) {
                    oldest = i;
                    oldestIsFree = true;
                    oldestTime = slots[i].startTime;
                } else {
                    if (oldestIsFree) continue;
                    oldest = i;
                    oldestTime = slots[i].startTime;
                }
            } else if (!slots[i].active && !oldestIsFree) {
                oldest = i;
                oldestIsFree = true;
                oldestTime = slots[i].startTime;
            }
        }
        slots[oldest] = { true, note, time };
        return oldest;
    }

    int release(uint8_t note) {
        for (int i = 0; i < POLYPHONY; ++i) {
            if (slots[i].active && slots[i].note == note) {
                slots[i].active = false;
                return i; // return the voice that was released
            }
        }
        return -1;
    }
};