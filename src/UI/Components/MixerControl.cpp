#include "UI/Components/MixerControl.hpp"

MixerControl::MixerControl(Mixer* mixer, QSize bounds)
    : mixer(mixer) {
    this->parentComponent = mixer;

    this->setFixedSize(bounds.width(), bounds.height());

    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* slidersLayout = new QHBoxLayout;

    for (int i = 0; i < 4; i++) {
        QVBoxLayout* singleSliderLayout = new QVBoxLayout;
        QSlider* slider = new QSlider(Qt::Vertical);
        QLabel* label = new QLabel(QString("%1 : %2").arg(i + 1).arg(mixer->inLevels[i] * 100));

        slider->setMinimum(0);
        slider->setMaximum(100);
        slider->setValue(mixer->inLevels[i] * 100);

        QObject::connect(slider, &QSlider::valueChanged, [mixer, label, i](int value) {
            mixer->inLevels[i] = value / 100.0f;
            label->setText(QString("%1 : %2").arg(i + 1).arg(mixer->inLevels[i] * 100));
        });

        singleSliderLayout->addWidget(label);
        singleSliderLayout->addWidget(slider);

        slidersLayout->addLayout(singleSliderLayout);
    }

    QPushButton* button = new QPushButton("Connect", this);

    connect(button, &QPushButton::clicked, this, [this]() {
        QMenu* menu = availableInputsMenu();

        menu->exec(QCursor::pos());
    });

    layout->addLayout(slidersLayout);
    layout->addWidget(button);
    setLayout(layout);
}