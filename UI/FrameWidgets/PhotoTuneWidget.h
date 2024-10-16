#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>
#include <UI/Utility.h>

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
  virtual void processAngleChanged(qreal rotation_delta, QPointF center) = 0;
  virtual void processScaleChanged(qreal scale, QPointF center) = 0;
  virtual void processLongTap(QTapAndHoldGesture *) = 0;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) = 0;
  virtual void grabWidgetGesture(Qt::GestureType gesture) = 0;

private:
  bool gestureEvent(QGestureEvent *event);
  void panTriggered(QPanGesture *);
  void pinchTriggered(QPinchGesture *);
  void longTapTriggered(QTapAndHoldGesture *);
  bool toucheEvent(QTouchEvent *touch);

private:
  bool is_gesture_moving_ = false;
  bool is_zooming_ = false;
  bool is_touch_movng_ = false;
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
  virtual double scaleFactor() const;

protected:
  struct PhotoPosition {
    //   PhotoPosition()
    //    {};
    std::optional<double> angle;
    std::optional<double> scale;
    std::optional<QPointF> offset;
    std::optional<QPointF> center;
    //  QRectF dest_rect;

    void reset() {
      offset.reset();
      scale.reset();
      angle.reset();
      center.reset();
    }

    void reset_exept_center() {
      offset.reset();
      scale.reset();
      angle.reset();
    }
  };
  QTransform getTransformForWidget(const PhotoPosition &photo_position);
  // QTransform getTransformForWidget() const ;

protected:
  PhotoPosition photo_position_;
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
  virtual double scaleFactor() const override final;

protected:
  void updatePhotoPosition(std::optional<QPointF> pos_delta,
                           std::optional<double> scale_factor,
                           std::optional<double> angle_delta,
                           std::optional<QPointF> center);

private:
  bool checkBoundares(QPointF delta, double scale, double angle) const;
};

class Frame {
public:
  Frame &operator=(const Frame &) = delete;

protected:
  void init(const FrameParameters &frame_data, QRectF widget_rect);
  void drawFrame(QPainter &);
  QRectF frameRect();

private:
  FrameParameters frame_data_;
  QRectF frame_boundary_rect_;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoTuneWidget class
///
class TouchButton;
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
  void SignalOpenFile();

public:
  void setPhoto(int id, const FrameParameters &frame_data,
                const Core::PhotoData &photo);
  void updatePhoto(const Core::PhotoData &photo);
  Core::PhotoData getPhoto() const;
  int getPhotoId() const;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta) override;
  virtual void processAngleChanged(qreal rotation_delta,
                                   QPointF center) override;
  virtual void processScaleChanged(qreal scale, QPointF center) override;
  virtual void processLongTap(QTapAndHoldGesture *) override;
  virtual bool processToucheEvent(const QList<QEventPoint> &points) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;

protected:
  // QWidget
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  void grabGestures(const QList<Qt::GestureType> &gestures);
  void updatePhoto(std::optional<QPointF> pos_delta,
                   std::optional<double> scale_factor,
                   std::optional<double> angle_delta,
                   std::optional<QPointF> center);

private:
  int id_;
  QPixmap background_;
  TouchButton *close_ = nullptr;
  TouchButton *open_file_ = nullptr;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
