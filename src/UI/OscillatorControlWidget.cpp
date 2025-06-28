#include "UI/OscillatorControlWidget.hpp"

OscillatorControlWidget::OscillatorControlWidget(Oscillator* oscillator, QWidget* parent)
    : QWidget(parent), oscillator(oscillator) {

    QVBoxLayout* layout = new QVBoxLayout(this);

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

    // --- Octave Slider + Label ---
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

    // --- Detune Slider + Label ---
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