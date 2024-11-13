#include "TouchButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

TouchButton::TouchButton(QWidget *parent) : QPushButton(parent) {
  setAttribute(Qt::WA_AcceptTouchEvents);
}

bool TouchButton::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {
    auto points = static_cast<QTouchEvent *>(event)->points();
    QPointF center;

    for (const QTouchEvent::TouchPoint &touchPoint : points) {
      center += touchPoint.position();
    }
    if (!points.empty()) {
      center /= points.size();
      if (geometry().contains(center.toPoint())) {
        return QPushButton::event(event);
      }
    }
    return false;
  }
  default:
    return QPushButton::event(event);
  }
  return QPushButton::event(event);
}

} // namespace FirstYear::UI
