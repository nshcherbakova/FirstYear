#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QWidget>
#include <UI/FrameWidgets/Photo/PhotoProcessor.h>
#include <UI/FrameWidgets/Touch/GestureProcessor.h>
#include <UI/Utility.h>

QT_BEGIN_NAMESPACE
class QEventPoint;
class QSvgRenderer;

QT_END_NAMESPACE

namespace FirstYear::UI {

class Frame {
public:
  Frame &operator=(const Frame &) = delete;

protected:
  void init(const FrameParameters &frame_data, QRectF widget_rect);
  void drawFrame(QPainter &);
  QRectF frameRect();

protected:
  FrameParameters frame_data_;

private:
  QRectF frame_boundary_rect_;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoTuneWidget class
///
class TextButton;
class TouchClickableLabel;
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
  void SignalTuneNextImage();
  void SignalTunePrevImage();
  void SignalOpenFile();
  void SignalTextClicked(QString text);

public:
  void setPhoto(int id, const FrameParameters &frame_data,
                const Core::PhotoDataPtr &photo, QString text);
  void updatePhoto(const Core::PhotoDataPtr &photo);
  void updateText(QString text);
  Core::PhotoDataPtr getPhoto() const;
  int getPhotoId() const;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta, QPointF global_point) override;
  virtual void processAngleChanged(qreal rotation_delta,
                                   QPointF center) override;
  virtual void processScaleChanged(qreal scale, QPointF center) override;
  virtual void
  processToucheEvent(QPointF delta,
                     const std::optional<QPointF> &touch_point) override;
  virtual void processSwipe(QSwipeGesture *) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;
  virtual void processDoubleTap(QPointF center) override;

protected:
  // QWidget
  virtual bool event(QEvent *event) override;
  virtual void paintEvent(QPaintEvent *) override;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
  virtual void wheelEvent(QWheelEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override;
  virtual void setVisible(bool visible) override;

private:
  void updatePhoto(std::optional<QPointF> pos_delta,
                   std::optional<double> scale_factor,
                   std::optional<double> angle_delta,
                   std::optional<QPointF> center);
  void redrawBackgroundImage();

private:
  int id_;
  // QPixmap background_;
  QSvgRenderer *background_ = nullptr;
  QPixmap background_image_;

  TextButton *open_file_ = nullptr;
  TextButton *close_ = nullptr;
  TextButton *next_ = nullptr;
  TextButton *prev_ = nullptr;
  TouchClickableLabel *text_ = nullptr;

  const double dpr_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
