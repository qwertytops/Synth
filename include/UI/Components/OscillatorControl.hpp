#pragma once

#include <QWidget>
#include <QSlider>
#include <QDial>
#include <QLabel>
#include <QBoxLayout>
#include <QSize>
#include <QMenu>
#include <QPushButton>

#include <string>

#include "Oscillator.hpp"
#include "UI/Control.hpp"

class OscillatorControl : public Control {
public:
    explicit OscillatorControl(Oscillator *oscillator, QSize bounds);

private:
    Oscillator* oscillator;
};