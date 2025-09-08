#pragma once

#include "SynthComponent.hpp"
// class Component;

class Connection {
public:
    bool visited = false;
    SynthComponent* source;
    NoteInput* destination;

    Connection(SynthComponent* source, NoteInput* destination)
        : source(source), destination(destination) {}
};