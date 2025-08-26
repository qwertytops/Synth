#include "UI/Components/OscillatorControlWidget.hpp"
#include "Input.hpp"
#include "Connection.hpp"

OscillatorControlWidget::OscillatorControlWidget(Oscillator* oscillator, QSize bounds, QWidget* parent)
    : QWidget(parent), oscillator(oscillator) {

    this->setFixedSize(bounds.width(), bounds.height());

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* levelAndWaveLayout = new QHBoxLayout;

    // Level Slider
    QVBoxLayout* levelLayout = new QVBoxLayout;
    QLabel* levelLabel = new QLabel(QString("Level: %1 %").arg(oscillator->level * 100));
    QSlider* levelSlider = new QSlider(Qt::Horizontal);
    levelSlider->setTickInterval(1);
    levelSlider->setTickPosition(QSlider::TicksAbove);
    levelSlider->setMinimum(0);
    levelSlider->setMaximum(10);
    levelSlider->setValue(oscillator->level * 10);

    levelLayout->addWidget(levelLabel);
    levelLayout->addWidget(levelSlider);
    levelAndWaveLayout->addLayout(levelLayout);

    QObject::connect(levelSlider, &QSlider::valueChanged, [oscillator, levelSlider, levelLabel](int value) {
        oscillator->level = value / (float)10;
        levelLabel->setText(QString("Level: %1 %").arg(levelSlider->value() * 10));
    });

    // --- WaveType Slider + Label ---
    QVBoxLayout* waveLayout = new QVBoxLayout;
    QLabel* waveLabel = new QLabel(QString("Wave: %1").arg(waveTypeToString(oscillator->waveType)));
    QSlider* waveSlider = new QSlider(Qt::Horizontal);
    waveSlider->setTickInterval(1);
    waveSlider->setTickPosition(QSlider::TicksAbove);
    waveSlider->setMinimum(0);
    waveSlider->setMaximum(waveTypes.size() - 1);
    waveSlider->setValue(static_cast<int>(oscillator->waveType));

    waveLayout->addWidget(waveLabel);
    waveLayout->addWidget(waveSlider);

    levelAndWaveLayout->addLayout(waveLayout);

    layout->addLayout(levelAndWaveLayout);

    QObject::connect(waveSlider, &QSlider::valueChanged, [oscillator, waveLabel](int value) {
        oscillator->waveType = waveTypes[value];
        waveLabel->setText(QString("Wave: %1").arg(waveTypeToString(oscillator->waveType)));
    });

    QHBoxLayout* frequencyControls = new QHBoxLayout;
    layout->addLayout(frequencyControls);

    // --- Octave Dial + Label ---
    QVBoxLayout* octaveLayout = new QVBoxLayout;
    QLabel* octaveLabel = new QLabel(QString("Octave: %1").arg(oscillator->octave));
    QDial* octaveDial = new QDial();
    octaveDial->setNotchesVisible(true);
    octaveDial->setMinimum(0);
    octaveDial->setMaximum(9);
    octaveDial->setValue(oscillator->octave);

    octaveLayout->addWidget(octaveLabel);
    octaveLayout->addWidget(octaveDial);
    frequencyControls->addLayout(octaveLayout);

    QObject::connect(octaveDial, &QDial::valueChanged, [oscillator, octaveLabel](int value) {
        oscillator->octave = value;
        octaveLabel->setText(QString("Octave: %1").arg(value));
    });

    // --- Detune Dial + Label ---
    QVBoxLayout* detuneLayout = new QVBoxLayout;
    QLabel* detuneLabel = new QLabel(QString("Detune: %1 c").arg(oscillator->detune));
    QDial* detuneDial = new QDial();
    detuneDial->setNotchesVisible(false);
    detuneDial->setMinimum(-49);
    detuneDial->setMaximum(50);
    detuneDial->setValue(oscillator->octave);

    detuneLayout->addWidget(detuneLabel);
    detuneLayout->addWidget(detuneDial);
    frequencyControls->addLayout(detuneLayout);

    QObject::connect(detuneDial, &QDial::valueChanged, [oscillator, detuneLabel](int value) {
        oscillator->detune = value;
        detuneLabel->setText(QString("Detune: %1 c").arg(value));
    });


    QPushButton* button = new QPushButton("Output: --", this);

    connect(button, &QPushButton::clicked, this, [this]() {
        QMenu* menu = new QMenu(this);

        // add mainOut to menu
        QAction* action = menu->addAction("Main Out");
        connect(action, &QAction::triggered, this, [this]() {
            Input* selected = this->oscillator->synth->mainOut;
            Connection* conn = new Connection(this->oscillator, selected);
            this->oscillator->outgoingConnections.push_back(conn);
        });
        
        // add all other components' inputs
        for (auto& comp : this->oscillator->synth->components) {
            QMenu* submenu = menu->addMenu(QString::fromStdString(comp->name) + QString(" %1").arg(comp->id));
            for (int i = 0; i < comp->inputs.size(); ++i) {
                QAction* action = submenu->addAction(QString::fromStdString(comp->inputs[i]->name));
                action->setData(i); // store index of Input
                connect(action, &QAction::triggered, this, [this, comp, i]() {
                    Input* selected = comp->inputs[i];
                    Connection* conn = new Connection(this->oscillator, selected);
                    this->oscillator->outgoingConnections.push_back(conn);
                });
            }
        }
        

        menu->exec(QCursor::pos());
    });

    layout->addWidget(button);
}