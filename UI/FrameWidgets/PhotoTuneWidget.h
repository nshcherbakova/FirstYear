#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <Core/Project/Project.h>
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
  bool processEvent(QEvent *event);
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
  qreal currentStepScaleFactor_ = 1;

private:
  bool is_gesture_moving_ = false;
  bool is_zooming_ = false;
  bool is_touch_movng_ = false;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoProcessor class
///

class PhotoProcessor {
public:
  PhotoProcessor &operator=(const PhotoProcessor &) = delete;

protected:
  virtual double currentStepScaleFactor() const = 0;
  virtual QRectF widgetRect() const = 0;

protected:
  void init(const Core::PhotoData &photo, QRectF boundary_rect);
  void drawPhoto(QPainter &);
  void updatePhotoPosition(QPointF pos_delta, double scale_factor,
                           double angle_delta);

private:
  bool checkBoundares(QPointF delta, double scale, double angle) const;
  QPointF toImageCoordinates(QPointF point) const;
  QTransform getTransformForWidget(QPointF point, double scale,
                                   double angle) const;

protected:
  Core::PhotoData photo_;

private:
  double internal_scale_ = 0;
  QRectF boundary_rect_;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The Frame class
///

class Frame {
public:
  Frame &operator=(const Frame &) = delete;

protected:
  void init(QSizeF frame_size, QRectF widget_rect);
  void drawFrame(QPainter &);
  QRectF frameRect();

private:
  QRectF frame_;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoTuneWidget class
///

class PhotoTuneWidget final : public QWidget,
                              public GestureProcessor,
                              public PhotoProcessor,
                              public Frame {
  Q_OBJECT
public:
  explicit PhotoTuneWidget(QWidget &parent);
  PhotoTuneWidget &operator=(const PhotoTuneWidget &) = delete;

signals:
  void SignalImageTuned();
  void SignalPhotoChanged();

public:
  void setPhoto(int id, QSizeF frame_size, const Core::PhotoData &photo);
  Core::PhotoData getPhoto() const;
  int getPhotoId() const;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta) override;
  virtual void processAngleChanged(qreal rotation_delta) override;
  virtual void processScaleChanged(qreal scale) override;
  virtual void processLongTap(QTapAndHoldGesture *) override;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;

private:
  // PhotoProcessor
  virtual double currentStepScaleFactor() const override;
  virtual QRectF widgetRect() const override;

protected:
  // QWidget
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
  void grabGestures(const QList<Qt::GestureType> &gestures);
  void updatePhoto(QPointF pos_delta, double scale_factor, double angle_delta);

private:
  int id_ = 0;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
