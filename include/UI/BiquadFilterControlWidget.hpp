#pragma once

#include <QWidget>
#include <QSlider>
#include <QDial>
#include <QLabel>
#include <QBoxLayout>
#include <QComboBox>

#include "BiquadFilter.hpp"

class BiquadFilterControlWidget : public QWidget {
public:
    explicit BiquadFilterControlWidget(BiquadFilter* filter, QWidget* parent = nullptr);

private:
    BiquadFilter* filter;
};