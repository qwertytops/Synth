#pragma once

#include <QWidget>
#include <QDial>
#include <QLabel>
#include <QBoxLayout>

#include "LPF.hpp"

class LPFControl : public QWidget {
public:
    explicit LPFControl(LPF* lpf, QWidget* parent = nullptr);

private:
    LPF* lpf;
};