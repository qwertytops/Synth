#pragma once

#include <vector>
#include <utility>
#include <string>

#include "Note.hpp"

using namespace std;
class NoteInput;
class Connection;
class Synth;

class SynthComponent {
public:
    string name;
    int id;
    vector<NoteInput*> inputs = {};
    vector<Connection*> incomingConnections = {};
    vector<Connection*> outgoingConnections = {};
    virtual void run(double elapsed) = 0;

    Synth* synth;
protected:
    vector<NoteInput*> destinations = {};
};