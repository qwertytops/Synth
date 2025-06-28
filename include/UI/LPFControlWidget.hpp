#pragma once

#include <QWidget>
#include <QDial>
#include <QLabel>
#include <QBoxLayout>

#include "LPF.hpp"

class LPFControlWidget : public QWidget {
public:
    explicit LPFControlWidget(LPF* lpf, QWidget* parent = nullptr);

private:
    LPF* lpf;
};