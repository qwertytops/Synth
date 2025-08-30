#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>

#include <string>

#include "Synth.hpp"

class SynthControl : public QWidget {

public:
    explicit SynthControl(Synth* synth, QWidget* parent = nullptr);

private:
    Synth* synth;
};