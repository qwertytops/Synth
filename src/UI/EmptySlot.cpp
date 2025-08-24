#include "UI/EmptySlot.hpp"

EmptySlot::EmptySlot(Synth* synth)
    : synth(synth) {
    this->setFlat(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(
        "QPushButton {"
        "background-color: #333; "
        "color: white; "
        "font-size: 40px; "
        "text-align: center; "
        "border: 1px solid #555;"
        "}"
        "QPushButton:hover {"
        "   color: rgba(73, 132, 87, 1);"
        "}"
        "QPushButton:pressed {"
        "   color: rgba(0, 102, 24, 1);"
        "}"
    );
    this->setText("+");
    this->setMinimumHeight(300);
    this->setMinimumWidth(250);

    QObject::connect(this, &QPushButton::clicked, [this]() {
        onClick();
    });

    initialiseComponentMenu();
}

void EmptySlot::onClick() {
    qWarning("Showing menu");
    menu->exec(QCursor::pos());
}

void EmptySlot::initialiseComponentMenu() {
    menu = new QMenu("Components", this);

    QString filePath = "../data/components.txt";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open file!");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QString className = line;
        QAction* action = menu->addAction(className);

        QObject::connect(action, &QAction::triggered, [this, className]() {
            SynthComponent* comp = SynthComponentFactory::instance().create(className.toStdString());
            if (comp) {
                // synth->components.push_back(comp);
            } else {
                qWarning("Class '%s' not found in registry", qPrintable(className));
            }
        });
    }
}