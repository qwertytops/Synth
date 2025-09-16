#include "UI/Components/BiquadFilterControl.hpp"
#include "NoteInput.hpp"
#include "Connection.hpp"

BiquadFilterControl::BiquadFilterControl(BiquadFilter* filter, QSize bounds)
    : filter(filter) {
    this->parentComponent = filter;

    this->setFixedSize(bounds.width(), bounds.height());

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* controlsLayout = new QHBoxLayout;

    // Title
    QHBoxLayout* titleLayout = new QHBoxLayout;
    QLabel* title = new QLabel("<b>Filter</b>"); // crashed here once for some reason
    title->setTextFormat(Qt::RichText);
    QPushButton* bypassButton = new QPushButton;
    bypassButton->setText("Bypass");
    bypassButton->setCheckable(true);
    bypassButton->setChecked(false);
    bypassButton->setMaximumWidth(100);

    QObject::connect(bypassButton, &QPushButton::toggled, [filter](bool value) {
        filter->setBypass(value);
    });

    titleLayout->addWidget(title);
    titleLayout->addWidget(bypassButton);

    // Frequency
    QVBoxLayout* freqLayout = new QVBoxLayout;
    QLabel* freqLabel = new QLabel(QString("Frequency: %1 Hz").arg(filter->getFrequency()));
    // DragOnlyDial* freqDial = new DragOnlyDial;
    QDial* freqDial = new QDial;
    freqDial->setMinimum(20);
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
    // DragOnlyDial* qDial = new DragOnlyDial;
    QDial* qDial = new QDial;
    qDial->setMinimum(1);
    qDial->setMaximum(100);
    qDial->setValue(filter->getQ());

    QObject::connect(qDial, &QSlider::valueChanged, [filter, qLabel](int value) {
        filter->setQ(value / 10.0);
        qLabel->setText(QString("Q: %1").arg(value / 10.0));
    });

    qLayout->addWidget(qLabel);
    qLayout->addWidget(qDial);

    // Gain
    QVBoxLayout* gainLayout = new QVBoxLayout;
    QLabel* gainLabel = new QLabel(QString("Gain: %1 dB").arg(filter->getGain() / 2.0));
    // DragOnlyDial* gainDial = new DragOnlyDial;
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

    // Assemble layout
    controlsLayout->addLayout(freqLayout);
    controlsLayout->addLayout(qLayout);
    controlsLayout->addLayout(gainLayout);
    controlsLayout->addLayout(modeLayout);

    QPushButton* button = new QPushButton("Connect", this);

    connect(button, &QPushButton::clicked, this, [this]() {
        QMenu* menu = availableInputsMenu();

        menu->exec(QCursor::pos());
    });

    layout->addLayout(titleLayout);
    layout->addLayout(controlsLayout);
    layout->addWidget(button);
}