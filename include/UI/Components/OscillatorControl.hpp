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
    // Q_OBJECT

public:
    explicit OscillatorControl(Oscillator* oscillator, QSize bounds, QWidget* parent = nullptr);

void NewFunction(Oscillator * oscillator);

private:
    Oscillator* oscillator;
};