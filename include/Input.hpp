#pragma once

#include <string>
#include <vector>
#include <utility>

#include "SynthComponent.hpp"

using namespace std;

class Input {
public:
    vector<pair<Note*, double>> pairs = {};

    string name;
    SynthComponent* parent;

    void reset() {
        pairs.clear();
    }

    Input(string name, SynthComponent* parent = nullptr)
        : name(name), parent(parent) {}

private:
};