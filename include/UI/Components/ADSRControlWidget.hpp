#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QSize>

#include "ADSR.hpp"

class ADSRControl : public QWidget {
public:
    explicit ADSRControl(ADSR* adsr, QSize bounds, QWidget* parent = nullptr);

private:
    ADSR* adsr;
};