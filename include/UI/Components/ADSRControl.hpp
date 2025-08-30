#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QSize>
#include <QPushButton>

#include "ADSR.hpp"
#include "UI/Control.hpp"

class ADSRControl : public Control {
public:
    explicit ADSRControl(ADSR* adsr, QSize bounds);

private:
    ADSR* adsr;
};