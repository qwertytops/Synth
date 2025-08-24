#pragma once

#include <vector>
#include <utility>

#include "Note.hpp"

using namespace std;
class Input;
class Connection;

class SynthComponent {
public:
    vector<Input*> inputs = {};
    vector<Connection*> incomingConnections = {};
    vector<Connection*> outgoingConnections = {};
    virtual void run(double elapsed) = 0;

protected:
    // Synth* synth;
    vector<Input*> destinations = {};
};