#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <Core/Project/Project.h>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QGestureEvent;
class QPanGesture;
class QPinchGesture;
// class QSwipeGesture;
class QTapAndHoldGesture;
class QTouchEvent;
class QEventPoint;

QT_END_NAMESPACE

namespace FirstYear::UI {

class GestureProcessor {

protected:
  void Initialise();
  GestureProcessor &operator=(const GestureProcessor &) = delete;
  void grabGestures(const QList<Qt::GestureType> &gestures);

protected:
  bool event(QEvent *event);
  virtual void processPan(QPointF delta) = 0;
  virtual void processAngleChanged(qreal rotation_delta) = 0;
  virtual void processScaleChanged(qreal scale) = 0;
  virtual void processLongTap(QTapAndHoldGesture *) = 0;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) = 0;
  virtual void grabWidgetGesture(Qt::GestureType gesture) = 0;

private:
  bool gestureEvent(QGestureEvent *event);
  void panTriggered(QPanGesture *);
  void pinchTriggered(QPinchGesture *);
  void longTapTriggered(QTapAndHoldGesture *);
  bool toucheEvent(QTouchEvent *touch);

protected:
  qreal currentStepScaleFactor = 1;

private:
  bool is_gesture_moving_ = false;
  bool is_zooming_ = false;
  bool is_touch_movng_ = false;
};

class PhotoTuneWidget final : public QWidget, public GestureProcessor {
  Q_OBJECT
public:
  explicit PhotoTuneWidget(QWidget &parent);
  PhotoTuneWidget &operator=(const PhotoTuneWidget &) = delete;
  void grabGestures(const QList<Qt::GestureType> &gestures);
signals:
  void SignalImageTuned();
  void SignalPhotoChanged();

public:
  void setPhoto(int id, const Core::PhotoData &photo);
  Core::PhotoData getPhoto() const;
  int getPhotoId() const;

protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta) override;
  virtual void processAngleChanged(qreal rotation_delta) override;
  virtual void processScaleChanged(qreal scale) override;
  virtual void processLongTap(QTapAndHoldGesture *) override;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;

private:
  bool checkBoundares(QPointF delta, double scale, double angle);
  void updatePhotoPosition(QPointF pos_delta, double scale_factor,
                           double angle_delta);
  QPointF toImageCoordinates(QPointF point);
  QTransform getTransformForWidget(QPointF point, double scale,
                                   double angle) const;
  QTransform getTransform(QPointF offset, double scale, double angle,
                          QRectF image_rect, QRectF frame_rect,
                          double internal_scale) const;

private:
  int id_ = 0;
  Core::PhotoData photo_;
  double internal_scale_ = 0;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
