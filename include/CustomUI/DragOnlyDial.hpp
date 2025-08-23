#pragma once

#include <QDial>
#include <QPoint>

#include <QWheelEvent>


class DragOnlyDial : public QDial {

public:
    explicit DragOnlyDial(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

};
