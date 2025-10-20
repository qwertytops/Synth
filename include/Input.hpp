#pragma once

#include <string>
#include <array>
#include <utility>

#include "Globals.hpp"
#include "SynthComponent.hpp"

using namespace std;

class Input {
public:
    array<pair<int, double>, POLYPHONY> pairs = {}; // voice, value

    string name;
    SynthComponent* parent;
    int endIndex;

    void add(pair<int, double> pair) {
        cout << "here1" << endl;
        pairs.at(endIndex % POLYPHONY) = pair;
        endIndex++;
    }
    void reset() {
        endIndex = 0;
    }

    Input(string name, SynthComponent* parent = nullptr)
        : name(name), parent(parent), endIndex(0) {}

private:
};