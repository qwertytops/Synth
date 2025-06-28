#include "UI/ADSRControlWidget.hpp"

ADSRControlWidget::ADSRControlWidget(ADSR* adsr, QWidget* parent)
    : QWidget(parent), adsr(adsr) {

    QHBoxLayout* layout = new QHBoxLayout;
    
    // Attack
    QVBoxLayout* attackLayout = new QVBoxLayout;
    QSlider* attackSlider = new QSlider(Qt::Vertical);
    QLabel* attackLabel = new QLabel(QString("%1 ms").arg(adsr->attack * 1000));
    attackSlider->setMinimum(0);
    attackSlider->setMaximum(1000);
    attackSlider->setValue(adsr->attack * 1000);
    QObject::connect(attackSlider, &QSlider::valueChanged, [adsr, attackLabel](int value) {
        adsr->attack = value / 1000.0f;
        attackLabel->setText(QString("%1 ms").arg(adsr->attack * 1000));
    });
    attackLayout->addWidget(attackLabel);
    attackLayout->addWidget(attackSlider);
    layout->addLayout(attackLayout);

    // Decay
    QVBoxLayout* decayLayout = new QVBoxLayout;
    QSlider* decaySlider = new QSlider(Qt::Vertical);
    QLabel* decayLabel = new QLabel(QString("%1 ms").arg(adsr->decay * 1000));
    decaySlider->setMinimum(0);
    decaySlider->setMaximum(1000);
    decaySlider->setValue(adsr->decay * 1000);
    QObject::connect(decaySlider, &QSlider::valueChanged, [adsr, decayLabel](int value) {
        adsr->decay = value / 1000.0f;
        decayLabel->setText(QString("%1 ms").arg(adsr->decay * 1000));
    });
    decayLayout->addWidget(decayLabel);
    decayLayout->addWidget(decaySlider);
    layout->addLayout(decayLayout);

    // Sustain
    QVBoxLayout* sustainLayout = new QVBoxLayout;
    QSlider* sustainSlider = new QSlider(Qt::Vertical);
    QLabel* sustainLabel = new QLabel(QString("%1 %").arg(adsr->sustain * 100));
    sustainSlider->setMinimum(0);
    sustainSlider->setMaximum(100);
    sustainSlider->setValue(adsr->sustain * 100);
    QObject::connect(sustainSlider, &QSlider::valueChanged, [adsr, sustainLabel](int value) {
        adsr->sustain = value / 100.0f;
        sustainLabel->setText(QString("%1 %").arg(adsr->sustain * 100));
    });
    sustainLayout->addWidget(sustainLabel);
    sustainLayout->addWidget(sustainSlider);
    layout->addLayout(sustainLayout);

    // Release
    QVBoxLayout* releaseLayout = new QVBoxLayout;
    QSlider* releaseSlider = new QSlider(Qt::Vertical);
    QLabel* releaseLabel = new QLabel(QString("%1 ms").arg(adsr->release * 1000));
    releaseSlider->setMinimum(0);
    releaseSlider->setMaximum(1000);
    releaseSlider->setValue(adsr->release * 1000);
    QObject::connect(releaseSlider, &QSlider::valueChanged, [adsr, releaseLabel](int value) {
        adsr->release = value / 1000.0f;
        releaseLabel->setText(QString("%1 ms").arg(adsr->release * 1000));
    });
    releaseLayout->addWidget(releaseLabel);
    releaseLayout->addWidget(releaseSlider);
    layout->addLayout(releaseLayout);

    setLayout(layout);
}