#pragma once
#include <vector>
#include <string>

enum class FilterMode {
    LOWPASS,
    HIGHPASS,
    BANDPASS,
    NOTCH,
    PEAK,
    LOWSHELF,
    HIGHSHELF
};

inline const std::vector<FilterMode> filterModes = {
    FilterMode::LOWPASS,
    FilterMode::HIGHPASS,
    FilterMode::BANDPASS,
    FilterMode::NOTCH,
    FilterMode::PEAK,
    FilterMode::LOWSHELF,
    FilterMode::HIGHSHELF
};

inline std::string filterModeToString(FilterMode type) {
    switch (type) {
        case FilterMode::LOWPASS:   return "Low Pass";
        case FilterMode::HIGHPASS:  return "High Pass";
        case FilterMode::BANDPASS:  return "Band Pass";
        case FilterMode::NOTCH:     return "Notch";
        case FilterMode::PEAK:      return "Peak";
        case FilterMode::LOWSHELF:  return "Low Shelf";
        case FilterMode::HIGHSHELF: return "High Shelf";
        default:                   return "UNKNOWN";
    }
}