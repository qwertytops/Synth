#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QObject>

#include "Synth.hpp"
#include "Connection.hpp"

class Control : public QWidget {
public:
    SynthComponent* parentComponent;
protected:
    QMenu* inputsMenu();
};