#pragma once
#ifndef GESTURE_PROCESSOR_H
#define GESTURE_PROCESSOR_H

#include <QGesture>

QT_BEGIN_NAMESPACE
class QGestureEvent;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;
class QTouchEvent;
class QEventPoint;
QT_END_NAMESPACE

class DoubleTapRecognizer;
class DoubleTap;

namespace FirstYear::UI {

class GestureProcessor {

public:
  GestureProcessor();

protected:
  GestureProcessor &operator=(const GestureProcessor &) = delete;

protected:
  Qt::GestureType DoubleTapGestureType;

  void grabGestures(const QList<Qt::GestureType> &gestures);
  bool processEvent(QEvent *event);
  virtual void processPan(QPointF delta) = 0;
  virtual void processAngleChanged(qreal rotation_delta, QPointF center) = 0;
  virtual void processScaleChanged(qreal scale, QPointF center) = 0;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) = 0;
  virtual void processSwipe(QSwipeGesture *) = 0;
  virtual void processDoubleTap(QPointF center) = 0;
  virtual void grabWidgetGesture(Qt::GestureType gesture) = 0;

private:
  bool gestureEvent(QGestureEvent *event);
  void panTriggered(QPanGesture *);
  void pinchTriggered(QPinchGesture *);
  void swipeTriggered(QSwipeGesture *gesture);
  void doubleTapTriggered(DoubleTap *gesture);
  bool toucheEvent(QTouchEvent *touch);

private:
  bool is_gesture_moving_ = false;
  DoubleTapRecognizer *double_tap_recognizer_ = nullptr;

  //  bool is_touch_movng_ = false;
};

} // namespace FirstYear::UI
#endif
