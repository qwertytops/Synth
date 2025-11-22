#pragma once

#define POLYPHONY 16
#define EVENT_CAP 1024
#define SAMPLE_RATE 44100
#define MONO_VOICE POLYPHONY

enum PolyphonyMode {
    POLY,
    LEGATO,
    MONO
};