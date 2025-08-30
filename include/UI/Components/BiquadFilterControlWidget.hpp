#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QComboBox>
#include <QPushButton>

#include "CustomUI/DragOnlyDial.hpp"

#include "BiquadFilter.hpp"

class BiquadFilterControl : public QWidget {
public:
    explicit BiquadFilterControl(BiquadFilter* filter, QWidget* parent = nullptr);

private:
    BiquadFilter* filter;
};