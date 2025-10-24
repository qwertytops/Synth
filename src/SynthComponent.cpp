#include "SynthComponent.hpp"
#include <iostream>

void SynthComponent::sendMidiEvent(Event e) {
    if (!events.push(e)) {
        cout << "Exceeded event cap. Failed to push event at time " << e.time << endl;
    }
}

bool SynthComponent::consumeMidiEvent() {
    return events.pop(currentMidiEvent);
}