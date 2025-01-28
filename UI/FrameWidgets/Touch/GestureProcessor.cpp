#include "GestureProcessor.h"
#include "doubletap.h"
#include <stdafx.h>

namespace FirstYear::UI {

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

  if (is_zooming_ || is_gesture_moving_) {
    return false;
  }
  touch->accept();
  const auto touchPoints = touch->points();

  return processToucheEvent(touchPoints);
}

void GestureProcessor::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    is_gesture_moving_ = true;
    break;
  case Qt::GestureFinished:
  case Qt::GestureCanceled:
    is_gesture_moving_ = false;
    break;
  default:
    is_gesture_moving_ = false;
  }
#endif

  QPointF delta = gesture->delta();
  processPan(delta);
}

void GestureProcessor::pinchTriggered(QPinchGesture *gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::RotationAngleChanged) {
    qreal rotation_delta =
        gesture->rotationAngle() - gesture->lastRotationAngle();
    processAngleChanged(rotation_delta, gesture->centerPoint());
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    processScaleChanged(gesture->scaleFactor(), gesture->centerPoint());
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
