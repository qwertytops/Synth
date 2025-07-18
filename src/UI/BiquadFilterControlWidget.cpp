#include "UI/BiquadFilterControlWidget.hpp"

BiquadFilterControlWidget::BiquadFilterControlWidget(BiquadFilter *filter, QWidget *parent)
    : QWidget(parent), filter(filter) {

    QHBoxLayout* layout = new QHBoxLayout(this);

    // Frequency
    QVBoxLayout* freqLayout = new QVBoxLayout;
    QLabel* freqLabel = new QLabel(QString("Frequency: %1 Hz").arg(filter->getFrequency()));
    QDial* freqDial = new QDial;
    freqDial->setMinimum(0);
    freqDial->setMaximum(20000);
    freqDial->setValue(filter->getFrequency());

    QObject::connect(freqDial, &QSlider::valueChanged, [filter, freqLabel](int value) {
        filter->setFrequency(value);
        freqLabel->setText(QString("Frequency: %1 Hz").arg(filter->getFrequency()));
    });

    freqLayout->addWidget(freqLabel);
    freqLayout->addWidget(freqDial);

    // Q
    QVBoxLayout* qLayout = new QVBoxLayout;
    QLabel* qLabel = new QLabel(QString("Q: %1").arg(filter->getQ() / 10.0));
    QDial* qDial = new QDial;
    qDial->setMinimum(0.1 * 10);
    qDial->setMaximum(10 * 10);
    qDial->setValue(filter->getQ());

    QObject::connect(qDial, &QSlider::valueChanged, [filter, qLabel](int value) {
        filter->setQ(value * 10);
        qLabel->setText(QString("Q: %1").arg(filter->getQ() / 10.0));
    });

    qLayout->addWidget(qLabel);
    qLayout->addWidget(qDial);

    // Gain
    QVBoxLayout* gainLayout = new QVBoxLayout;
    QLabel* gainLabel = new QLabel(QString("Gain: %1 dB").arg(filter->getGain() / 2.0));
    QDial* gainDial = new QDial;
    gainDial->setMinimum(-10 * 2);
    gainDial->setMaximum(10 * 2);
    gainDial->setValue(filter->getGain());

    QObject::connect(gainDial, &QSlider::valueChanged, [filter, gainLabel](int value) {
        filter->setGain(value * 2);
        gainLabel->setText(QString("Gain: %1 dB").arg(filter->getGain() / 2.0));
    });

    gainLayout->addWidget(gainLabel);
    gainLayout->addWidget(gainDial);

    // Mode
    QVBoxLayout* modeLayout = new QVBoxLayout;
    QLabel* modeLabel = new QLabel("Filter Mode");
    QComboBox* modeSelector = new QComboBox;
    for (auto& mode : filterModes) {
        modeSelector->addItem(QString("%1").arg(filterModeToString(mode)));
    }
    modeSelector->setCurrentIndex(static_cast<int>(filter->getMode()));
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(modeSelector);

    QObject::connect(modeSelector, &QComboBox::currentIndexChanged, [filter](int value) {
        filter->setMode(static_cast<FilterMode>(value));
    });


    layout->addLayout(freqLayout);
    layout->addLayout(qLayout);
    layout->addLayout(gainLayout);
    layout->addLayout(modeLayout);
}