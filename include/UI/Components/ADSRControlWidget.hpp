#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>

#include "ADSR.hpp"

class ADSRControlWidget : public QWidget {
public:
    explicit ADSRControlWidget(ADSR* adsr, QWidget* parent = nullptr);

private:
    ADSR* adsr;
};