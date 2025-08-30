#include "UI/EmptySlot.hpp"

int EmptySlot::id = 0;

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
                if (auto osc = dynamic_cast<Oscillator*>(comp)) {
                    osc->synth = synth;
                    synth->addComponent(osc);
                    osc->id = id++;
                    QWidget* control = new OscillatorControl(osc, this->size());
                    replaceGridItem(control);
                } else if (auto env = dynamic_cast<ADSR*>(comp)) {
                    env->synth = synth;
                    synth->addComponent(env);
                    env->id = id++;
                    QWidget *control = new ADSRControl(env, this->size());
                    replaceGridItem(control);
                }
            } else {
                qWarning("Class '%s' not found in registry", qPrintable(className));
            }
        });
    }
}

void EmptySlot::replaceGridItem(QWidget* control) {
    // Get the grid layout
    auto* grid = qobject_cast<QGridLayout*>(this->parentWidget()->layout());
    if (!grid)
        return;

    // Find the current position of `this` in the grid
    int row = -1, col = -1, rowSpan = 1, colSpan = 1;
    bool found = false;
    for (int r = 0; r < grid->rowCount() && !found; ++r) {
        for (int c = 0; c < grid->columnCount(); ++c) {
            QLayoutItem* item = grid->itemAtPosition(r, c);
            if (item && item->widget() == this) {
                row = r;
                col = c;
                found = true;
                break;
            }
        }
    }

    if (!found)
        return;

    // Remove and delete the old widget
    QLayoutItem* item = grid->takeAt(grid->indexOf(this));
    delete item;             // removes layout item
    this->hide();
    this->deleteLater();     // schedule deletion

    // Place the new widget in the same grid cell
    grid->addWidget(control, row, col, rowSpan, colSpan);
}