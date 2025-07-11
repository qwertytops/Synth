#pragma once

#include <QWidget>
#include <QSlider>
#include <QDial>
#include <QLabel>
#include <QBoxLayout>

#include <string>

#include "Oscillator.hpp"

class OscillatorControlWidget : public QWidget {
    // Q_OBJECT

public:
    explicit OscillatorControlWidget(Oscillator* oscillator, QWidget* parent = nullptr);

private:
    Oscillator* oscillator;
};