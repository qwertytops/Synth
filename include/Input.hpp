#include <string>
#include <vector>
#include <utility>

#include "SynthComponent.hpp"

using namespace std;

class Input {
public:
    vector<pair<Note*, double>> pairs = {};

    SynthComponent* parent;
    string name;

    void reset() {
        pairs.clear();
    }
private:

};