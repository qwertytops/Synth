#pragma once

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QSize>
#include <QBoxLayout>
#include <QLabel>

#include "Mixer.hpp"
#include "UI/Control.hpp"

class MixerControl : public Control {
public:
    explicit MixerControl(Mixer* mixer, QSize bounds);
private:
    Mixer* mixer;
};