#pragma once
#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QWidget>

class QGestureEvent;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;
class QTapAndHoldGesture;
class QTouchEvent;
class QEventPoint;

namespace FirstYear::UI {
class TouchButton;
}

namespace FirstYear::UI::Preview {

class GestureProcessor {

protected:
  void Initialise();
  GestureProcessor &operator=(const GestureProcessor &) = delete;
  void grabGestures(const QList<Qt::GestureType> &gestures);

protected:
  bool processEvent(QEvent *event);
  virtual void processPan(QPointF delta) = 0;
  virtual void processScaleChanged(qreal scale, QPointF center) = 0;
  virtual void processLongTap(QTapAndHoldGesture *) = 0;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) = 0;
  virtual void processSwipe(QSwipeGesture *) = 0;
  virtual void grabWidgetGesture(Qt::GestureType gesture) = 0;

private:
  bool gestureEvent(QGestureEvent *event);
  void panTriggered(QPanGesture *);
  void pinchTriggered(QPinchGesture *);
  void longTapTriggered(QTapAndHoldGesture *);
  void swipeTriggered(QSwipeGesture *gesture);
  bool toucheEvent(QTouchEvent *touch);

private:
  bool is_gesture_moving_ = false;
  bool is_zooming_ = false;
  //  bool is_touch_movng_ = false;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoPainter class
///

class PhotoPainter {
public:
  PhotoPainter &operator=(const PhotoPainter &) = delete;

public:
  void init(const Core::PhotoData &photo, QRectF destanation_rect,
            QRectF boundary_rect);
  void drawPhoto(QPainter &);

protected:
  struct PhotoPosition {
    //   PhotoPosition()
    //    {};
    std::optional<double> scale;
    std::optional<QPointF> offset;
    std::optional<QPointF> center;
    //  QRectF dest_rect;

    void reset() {
      offset.reset();
      scale.reset();
      center.reset();
    }

    void reset_exept_center() {
      offset.reset();
      scale.reset();
    }
  };
  QTransform getTransformForWidget(const PhotoPosition &photo_position,
                                   QTransform &transform_offset,
                                   QTransform &transform_scale_rotate);
  // QTransform getTransformForWidget() const ;

protected:
  QTransform transform_;
  Core::PhotoData photo_data_;

  double internal_scale_ = 1;
  QRectF boundary_rect_;
  QRectF destanation_rect_;
};
/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoProcessor class
///

class PhotoProcessor : protected PhotoPainter {
public:
  PhotoProcessor &operator=(const PhotoProcessor &) = delete;

protected:
  void updatePhotoPosition(std::optional<QPointF> pos_delta,
                           std::optional<double> scale_factor,
                           std::optional<QPointF> center);

private:
  bool checkBoundares(const QTransform &transform) const;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoTuneWidget class
///

class PreviewWidget final : public QWidget,
                            public GestureProcessor,
                            public PhotoProcessor {
  Q_OBJECT
public:
  explicit PreviewWidget(QWidget &parent);
  PreviewWidget &operator=(const PreviewWidget &) = delete;

signals:
  void SignalShareImage();

public:
  virtual void setVisible(bool visible) override;

public:
  void setImage(QPixmap image);
  QPixmap getImage() const;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta) override;
  virtual void processScaleChanged(qreal scale, QPointF center) override;
  virtual void processLongTap(QTapAndHoldGesture *) override;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) override;
  virtual void processSwipe(QSwipeGesture *) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;

protected:
  // QWidget
  virtual bool event(QEvent *event) override;
  virtual void paintEvent(QPaintEvent *) override;
  virtual void wheelEvent(QWheelEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override;

private:
  void grabGestures(const QList<Qt::GestureType> &gestures);
  void updatePhoto(const Core::PhotoData &photo);
  void updatePhoto(std::optional<QPointF> pos_delta,
                   std::optional<double> scale_factor,
                   std::optional<QPointF> center);

private:
  QPixmap image_;
  TouchButton *share_ = nullptr;
  TouchButton *close_ = nullptr;
};

} // namespace FirstYear::UI::Preview
#endif // PREVIEW_WIDGET_H
