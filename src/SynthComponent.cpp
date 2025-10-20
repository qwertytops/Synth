#include "SynthComponent.hpp"

void SynthComponent::sendMidiEvent(Event e) {
    events[nextFreeEventIndex++ % EVENT_CAP] = e;
}

bool SynthComponent::consumeMidiEvent() {
    if (nextEventToConsume >= nextFreeEventIndex) {
        return false;
    }
    currentMidiEvent = events[nextEventToConsume++];
    return true;
}