#pragma once

#include <vector>
#include <utility>
#include <string>

#include "Event.hpp"
#include "Globals.hpp"

using namespace std;
class Input;
class Connection;
class Synth;

class SynthComponent {
public:
    string name;
    int id;
    vector<Input*> inputs = {};
    vector<Connection*> incomingConnections = {};
    vector<Connection*> outgoingConnections = {};
    virtual void run(double elapsed) = 0;

    Synth* synth;

    PolyphonyMode polyphonyMode;
    void sendMidiEvent(Event e);

protected:
    vector<Input*> destinations = {};

    std::array<Event, EVENT_CAP> events = {};
    int nextFreeEventIndex = 0;
    int nextEventToConsume = 0;
    Event currentMidiEvent;

    bool consumeMidiEvent();
};