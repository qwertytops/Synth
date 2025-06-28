#pragma once

#include <cmath>
using namespace std;

class LPF {
public:
    int cutoff;
    
    double run(double input);
    void setCutoff(int cutoff);

    LPF(double cutoff);

private:
    double alpha;
    double previous;
    bool init;

    double calculateAlpha(int cutoff);
};