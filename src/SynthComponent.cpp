#include "SynthComponent.hpp"
#include "Connection.hpp"
#include "Input.hpp"

#include <iostream>

void SynthComponent::sendMidiEvent(Event e) {
    if (!events.push(e)) {
        cout << "Exceeded event cap. Failed to push event at time " << e.time << endl;
    }
}

bool SynthComponent::consumeMidiEvent() {
    return events.pop(currentMidiEvent);
}

SynthComponent::~SynthComponent() {    
    for (int i = incomingConnections.size() - 1; i >= 0; i--) {
        auto conn = incomingConnections.at(i);
        auto& s = conn->source->outgoingConnections;
        s.erase(std::remove(s.begin(), s.end(), conn), s.end());
        // cout << "deleting in conn " << conn << endl;
        delete conn;
    }

    for (int i = outgoingConnections.size() - 1; i >= 0; i--) {
        auto conn = outgoingConnections.at(i);
        auto& s = conn->destination->parent->incomingConnections;
        s.erase(std::remove(s.begin(), s.end(), conn), s.end());
        // cout << "deleting out conn " << conn << endl;
        delete conn;
    }

    for (int i = inputs.size() - 1; i >= 0; i--) {
        delete inputs.at(i);
    }
}