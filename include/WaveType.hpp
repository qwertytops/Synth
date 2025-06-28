#pragma once
#include <vector>
#include <string>

enum class WaveType {
    SINE,
    TRI,
    SQUARE,
    SAW,
    SAW2,
    NOISE
};
inline const std::vector<WaveType> waveTypes = {
    WaveType::SINE, WaveType::TRI, WaveType::SQUARE, WaveType::SAW, WaveType::SAW2, WaveType::NOISE
};

static const std::string waveTypeToString(WaveType type) {
    switch (type) {
        case WaveType::SINE:   return "SINE";
        case WaveType::TRI:    return "TRI";
        case WaveType::SQUARE: return "SQUARE";
        case WaveType::SAW:    return "SAW";
        case WaveType::SAW2:   return "SAW2";
        case WaveType::NOISE:  return "NOISE";
        default:               return "UNKNOWN";
    }
}