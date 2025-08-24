#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCursor>
#include <QObject>
#include <QMenu>
#include <QFile>

#include "SynthComponentFactory.hpp"
#include "Synth.hpp"

class EmptySlot : public QPushButton {
public:
    EmptySlot(Synth* synth);

private:
    Synth* synth;

    QMenu* menu;

    void onClick();
    void initialiseComponentMenu();
};