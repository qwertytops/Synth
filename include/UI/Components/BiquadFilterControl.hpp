#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QSize>

// #include "CustomUI/DragOnlyDial.hpp"
#include <QDial>

#include "BiquadFilter.hpp"
#include "UI/Control.hpp"

class BiquadFilterControl : public Control {
public:
    explicit BiquadFilterControl(BiquadFilter* filter, QSize bounds);

private:
    BiquadFilter* filter;
};