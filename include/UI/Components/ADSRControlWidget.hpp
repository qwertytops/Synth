#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QSize>

#include "ADSR.hpp"

class ADSRControlWidget : public QWidget {
public:
    explicit ADSRControlWidget(ADSR* adsr, QSize bounds, QWidget* parent = nullptr);

private:
    ADSR* adsr;
};