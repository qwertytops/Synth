#pragma once

#include <string>
#include <array>
#include <utility>

#include "Globals.hpp"
#include "SynthComponent.hpp"

using namespace std;

class Input {
public:
    array<double, POLYPHONY + 1> values = {}; // voice, value

    string name;
    SynthComponent* parent;

    void add(int voice, double value) {
        values.at(voice) += value;
    }
    void reset() {
        // this might not be necessary at all
        for (int i = 0; i < POLYPHONY + 1; i++) {
            values.at(i) = 0.0;
        }
    }

    Input(string name, SynthComponent* parent = nullptr)
        : name(name), parent(parent) {}

private:
};