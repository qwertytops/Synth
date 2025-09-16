#include "UI/Components/ADSRControl.hpp"
#include "NoteInput.hpp"
#include "Connection.hpp"

ADSRControl::ADSRControl(ADSR* adsr, QSize bounds)
    : adsr(adsr) {
    this->parentComponent = adsr;

    this->setFixedSize(bounds.width(), bounds.height());

    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* slidersLayout = new QHBoxLayout;
    
    // Attack
    QVBoxLayout* attackLayout = new QVBoxLayout;
    QSlider* attackSlider = new QSlider(Qt::Vertical);
    QLabel* attackLabel = new QLabel(QString("%1 ms").arg(adsr->attack * 1000)); // crash here once for some reason

    attackLabel->setMinimumWidth(60);
    attackSlider->setMinimumWidth(60);

    attackSlider->setMinimum(0);
    attackSlider->setMaximum(1000);
    attackSlider->setValue(adsr->attack * 1000);
    QObject::connect(attackSlider, &QSlider::valueChanged, [adsr, attackLabel](int value) {
        adsr->attack = value / 1000.0f;
        attackLabel->setText(QString("%1 ms").arg(adsr->attack * 1000));
    });
    attackLayout->addWidget(attackLabel);
    attackLayout->addWidget(attackSlider);
    slidersLayout->addLayout(attackLayout);

    // Decay
    QVBoxLayout* decayLayout = new QVBoxLayout;
    QSlider* decaySlider = new QSlider(Qt::Vertical);
    QLabel* decayLabel = new QLabel(QString("%1 ms").arg(adsr->decay * 1000));

    decayLabel->setMinimumWidth(60);
    decaySlider->setMinimumWidth(60);

    decaySlider->setMinimum(0);
    decaySlider->setMaximum(1000);
    decaySlider->setValue(adsr->decay * 1000);
    QObject::connect(decaySlider, &QSlider::valueChanged, [adsr, decayLabel](int value) {
        adsr->decay = value / 1000.0f;
        decayLabel->setText(QString("%1 ms").arg(adsr->decay * 1000));
    });
    decayLayout->addWidget(decayLabel);
    decayLayout->addWidget(decaySlider);
    slidersLayout->addLayout(decayLayout);

    // Sustain
    QVBoxLayout* sustainLayout = new QVBoxLayout;
    QSlider* sustainSlider = new QSlider(Qt::Vertical);
    QLabel* sustainLabel = new QLabel(QString("%1 %").arg(adsr->sustain * 100));

    sustainLabel->setMinimumWidth(60);
    sustainSlider->setMinimumWidth(60);

    sustainSlider->setMinimum(0);
    sustainSlider->setMaximum(100);
    sustainSlider->setValue(adsr->sustain * 100);
    QObject::connect(sustainSlider, &QSlider::valueChanged, [adsr, sustainLabel](int value) {
        adsr->sustain = value / 100.0f;
        sustainLabel->setText(QString("%1 %").arg(adsr->sustain * 100));
    });
    sustainLayout->addWidget(sustainLabel);
    sustainLayout->addWidget(sustainSlider);
    slidersLayout->addLayout(sustainLayout);

    // Release
    QVBoxLayout* releaseLayout = new QVBoxLayout;
    QSlider* releaseSlider = new QSlider(Qt::Vertical);
    QLabel* releaseLabel = new QLabel(QString("%1 ms").arg(adsr->release * 1000));

    releaseLabel->setMinimumWidth(60);
    releaseSlider->setMinimumWidth(60);

    releaseSlider->setMinimum(0);
    releaseSlider->setMaximum(1000);
    releaseSlider->setValue(adsr->release * 1000);
    QObject::connect(releaseSlider, &QSlider::valueChanged, [adsr, releaseLabel](int value) {
        adsr->release = value / 1000.0f;
        releaseLabel->setText(QString("%1 ms").arg(adsr->release * 1000));
    });
    releaseLayout->addWidget(releaseLabel);
    releaseLayout->addWidget(releaseSlider);
    slidersLayout->addLayout(releaseLayout);

    layout->addLayout(slidersLayout);

    QPushButton* button = new QPushButton("Connect", this);

    connect(button, &QPushButton::clicked, this, [this]() {
        QMenu* menu = availableInputsMenu();

        menu->exec(QCursor::pos());
    });

    layout->addWidget(button);
    setLayout(layout);
}