#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>

#include <string>

#include "Synth.hpp"

class SynthControlWidget : public QWidget {

public:
    explicit SynthControlWidget(Synth* synth, QWidget* parent = nullptr);

private:
    Synth* synth;
};