#include "UI/Components/OscillatorControlWidget.hpp"
#include "Input.hpp"
#include "Connection.hpp"

OscillatorControlWidget::OscillatorControlWidget(Oscillator* oscillator, QWidget* parent)
    : QWidget(parent), oscillator(oscillator) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Level Slider
    QLabel* levelLabel = new QLabel(QString("Level: %1 %").arg(oscillator->level * 100));
    QSlider* levelSlider = new QSlider(Qt::Horizontal);
    levelSlider->setTickInterval(1);
    levelSlider->setTickPosition(QSlider::TicksAbove);
    levelSlider->setMinimum(0);
    levelSlider->setMaximum(10);
    levelSlider->setValue(oscillator->level * 10);

    layout->addWidget(levelLabel);
    layout->addWidget(levelSlider);

    QObject::connect(levelSlider, &QSlider::valueChanged, [oscillator, levelSlider, levelLabel](int value) {
        oscillator->level = value / (float)10;
        levelLabel->setText(QString("Level: %1 %").arg(levelSlider->value() * 10));
    });

    // --- WaveType Slider + Label ---
    QLabel* waveLabel = new QLabel(QString("Wave: %1").arg(waveTypeToString(oscillator->waveType)));
    QSlider* waveSlider = new QSlider(Qt::Horizontal);
    waveSlider->setTickInterval(1);
    waveSlider->setTickPosition(QSlider::TicksAbove);
    waveSlider->setMinimum(0);
    waveSlider->setMaximum(waveTypes.size() - 1);
    waveSlider->setValue(static_cast<int>(oscillator->waveType));

    layout->addWidget(waveLabel);
    layout->addWidget(waveSlider);

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
}