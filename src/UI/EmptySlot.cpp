#include "UI/EmptySlot.hpp"

EmptySlot::EmptySlot() {
    this->setFlat(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(
        "background-color: #333; "
        "color: white; "
        "font-size: 40px; "
        "text-align: center; "
        "border: 1px solid #555;"
    );
    this->setText("+");
    this->setMinimumHeight(300);
    this->setMinimumWidth(250);
}