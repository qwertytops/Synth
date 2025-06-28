#include "UI/LPFControlWidget.hpp"

LPFControlWidget::LPFControlWidget(LPF* lpf, QWidget *parent)
    : QWidget(parent), lpf(lpf) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    

    QLabel* cutoffLabel = new QLabel(QString("Cutoff: %1 hz").arg(lpf->cutoff));
    QDial* cutoffDial = new QDial;

    cutoffDial->setNotchesVisible(false);
    cutoffDial->setMinimum(20);
    cutoffDial->setMaximum(20000);
    cutoffDial->setValue(lpf->cutoff);

    layout->addWidget(cutoffLabel);
    layout->addWidget(cutoffDial);

    QObject::connect(cutoffDial, &QDial::valueChanged, [lpf, cutoffLabel](int value) {
        lpf->setCutoff(value);
        cutoffLabel->setText(QString("Cutoff: %1 hz").arg(lpf->cutoff));
    });
}