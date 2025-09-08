#pragma once

#include <string>
#include <vector>
#include <utility>

#include "SynthComponent.hpp"

using namespace std;

class SignalInput {
public:
    double value;

    string name;
    SynthComponent* parent;

    void add(double val) {
        value += val;
    }

    void reset() {
        value = 0;
    }

    SignalInput(string name, SynthComponent* parent = nullptr)
        : name(name), parent(parent) {}

private:
};