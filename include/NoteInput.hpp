#pragma once

#include <string>
#include <array>
#include <utility>

#include "Globals.hpp"
#include "SynthComponent.hpp"

using namespace std;

class NoteInput {
public:
    array<pair<Note*, double>, POLYPHONY> pairs = {};

    string name;
    SynthComponent* parent;
    int endIndex;

    void add(pair<Note*, double> pair) {
        pairs.at(endIndex % POLYPHONY) = pair;
        endIndex++;
    }
    void reset() {
        endIndex = 0;
    }

    NoteInput(string name, SynthComponent* parent = nullptr)
        : name(name), parent(parent), endIndex(0) {}

private:
};