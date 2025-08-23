#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QComboBox>
#include <QPushButton>

#include "CustomUI/DragOnlyDial.hpp"

#include "BiquadFilter.hpp"

class BiquadFilterControlWidget : public QWidget {
public:
    explicit BiquadFilterControlWidget(BiquadFilter* filter, QWidget* parent = nullptr);

private:
    BiquadFilter* filter;
};