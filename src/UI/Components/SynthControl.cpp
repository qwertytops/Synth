#include "UI/Components/SynthControl.hpp"
#include "Input.hpp"
#include "Connection.hpp"

SynthControl::SynthControl(Synth* synth, QWidget* parent)
    : QWidget(parent), synth(synth) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Volume control
    QSlider* volumeSlider = new QSlider(Qt::Horizontal);
    QLabel* volumeLabel = new QLabel(QString("Volume: %1 %").arg(synth->volume * 100));
    volumeSlider->setTickInterval(1);
    volumeSlider->setTickPosition(QSlider::TicksAbove);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(10);
    volumeSlider->setValue(synth->volume * 10);

    layout->addWidget(volumeLabel);
    layout->addWidget(volumeSlider);

    QObject::connect(volumeSlider, &QSlider::valueChanged, [synth, volumeSlider, volumeLabel](int value) {
        synth->volume = value / (float)10;
        volumeLabel->setText(QString("Volume: %1 %").arg(volumeSlider->value() * 10));
    });
}