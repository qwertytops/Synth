#pragma once

#include <vector>
#include <utility>
#include <string>
#include <boost/lockfree/spsc_queue.hpp>

#include "Event.hpp"
#include "Globals.hpp"

using namespace std;
using namespace boost::lockfree;

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

    virtual ~SynthComponent();

protected:
    vector<Input*> destinations = {};

    boost::lockfree::spsc_queue<Event, boost::lockfree::capacity<EVENT_CAP>> events = {};
    Event currentMidiEvent;

    bool consumeMidiEvent();
};