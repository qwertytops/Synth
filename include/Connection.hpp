#pragma once

#include "Component.hpp"
// class Component;

class Connection {
public:
    bool visited = false;
    SynthComponent* source;
    Input* destination;

    Connection(SynthComponent* source, Input* destination)
        : source(source), destination(destination) {}
};