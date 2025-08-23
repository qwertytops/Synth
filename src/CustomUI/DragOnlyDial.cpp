#include "CustomUI/DragOnlyDial.hpp"

DragOnlyDial::DragOnlyDial(QWidget *parent) {

}

void DragOnlyDial::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void DragOnlyDial::mousePressEvent(QMouseEvent *event) {
    // ignore normal behaviour but register start location
}

void DragOnlyDial::mouseMoveEvent(QMouseEvent *event) {
    // move by Y distance from start locatoin
}

void DragOnlyDial::mouseReleaseEvent(QMouseEvent *event) {
    // stop moving
}
