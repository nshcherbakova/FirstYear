#include "GestureProcessor.h"
#include "doubletap.h"
#include <stdafx.h>

namespace FirstYear::UI {
static const double c_zoom_max = 0.2;
static const double c_pos_chenge_max = 20;
static const double c_angle_change_max = 30;

GestureProcessor::GestureProcessor() {

  double_tap_recognizer_ = new DoubleTapRecognizer();
  DoubleTapGestureType =
      QGestureRecognizer::registerRecognizer(double_tap_recognizer_);
}

void GestureProcessor::grabGestures(const QList<Qt::GestureType> &gestures) {
  for (Qt::GestureType gesture : gestures)
    grabWidgetGesture(gesture);
}

bool GestureProcessor::processEvent(QEvent *event) {

  switch (event->type()) {
  case QEvent::Gesture: {
    return gestureEvent(static_cast<QGestureEvent *>(event));
  }
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {

    toucheEvent(static_cast<QTouchEvent *>(event));
    return true;
  }
  default:
    return false;
  }
}

bool GestureProcessor::gestureEvent(QGestureEvent *event) {
  if (QGesture *pan = event->gesture(Qt::PanGesture))
    panTriggered(static_cast<QPanGesture *>(pan));
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinchTriggered(static_cast<QPinchGesture *>(pinch));
  if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
    swipeTriggered(static_cast<QSwipeGesture *>(swipe));
  if (QGesture *double_tap = event->gesture(DoubleTapGestureType)) {
    event->accept();
    doubleTapTriggered(static_cast<DoubleTap *>(double_tap));
  }
  return true;
}

bool GestureProcessor::toucheEvent(QTouchEvent *touch) {

  if (is_gesture_moving_) {
    return false;
  }
  touch->accept();
  const auto points = touch->points();

  QPointF delta;
  int count = 0;

  QPointF point;
  for (const QTouchEvent::TouchPoint &touchPoint : points) {
    switch (touchPoint.state()) {
    case QEventPoint::Stationary:
    case QEventPoint::Released:
    case QEventPoint::Pressed:

      // don't do anything if this touch point hasn't moved or has been released
      continue;
    default: {

      delta += touchPoint.position() - touchPoint.lastPosition();
      point += touchPoint.position();
    }
    }
    count += 1;
  }
  if (count > 0) {
    delta /= count;

    processToucheEvent(delta,
                       points.isEmpty() ? std::optional<QPointF>() : (point));
    return true;
  }
  return false;
}

void GestureProcessor::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    is_gesture_moving_ = false;
    break;
  case Qt::GestureFinished:
  case Qt::GestureCanceled:
    is_gesture_moving_ = false;
    break;
  default: {
    is_gesture_moving_ = false;
  }
  }
#endif

  QPointF delta = gesture->delta();

  if (delta.manhattanLength() > c_pos_chenge_max) {
    delta = QPoint(0, 0);
  }
  if (delta.manhattanLength() > c_pos_chenge_max / 2) {
    delta = delta / 2.0;
  }
  processPan(delta, gesture->hotSpot());
}

void GestureProcessor::pinchTriggered(QPinchGesture *gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::RotationAngleChanged) {
    qreal rotation_delta =
        gesture->rotationAngle() - gesture->lastRotationAngle();
    if (abs(rotation_delta) >= c_angle_change_max) {
      rotation_delta = 0.0;
    }

    processAngleChanged(rotation_delta, gesture->centerPoint());
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {

    const auto scale = fabs(gesture->scaleFactor() - 1.0) > c_zoom_max
                           ? 1.0
                           : gesture->scaleFactor();

    processScaleChanged(scale, gesture->centerPoint());
  }
  if (gesture->state() == Qt::GestureFinished) {

    processScaleChanged(gesture->scaleFactor() / gesture->lastScaleFactor(),
                        gesture->centerPoint());
  }
}

void GestureProcessor::swipeTriggered(QSwipeGesture *gesture) {
  if (gesture->state() == Qt::GestureFinished) {
    processSwipe(gesture);
  }
}

void GestureProcessor::doubleTapTriggered(DoubleTap *gesture) {
  if (gesture->state() == Qt::GestureFinished) {
    processDoubleTap(gesture->position());
  }
}

} // namespace FirstYear::UI
