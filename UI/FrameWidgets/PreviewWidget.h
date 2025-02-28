#pragma once
#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QWidget>
#include <UI/FrameWidgets/Photo/PhotoProcessor.h>
#include <UI/FrameWidgets/Touch/GestureProcessor.h>

class QEventPoint;
class QSvgRenderer;

namespace FirstYear::UI {
class TouchButton;
class TextButton;
} // namespace FirstYear::UI

namespace FirstYear::UI::Preview {

class PreviewWidget final : public QWidget,
                            public GestureProcessor,
                            public PhotoProcessor {
  Q_OBJECT
public:
  explicit PreviewWidget(QWidget &parent);
  PreviewWidget &operator=(const PreviewWidget &) = delete;

signals:
  void SignalShareImage();
  void SignalClosed();

public:
  virtual void setVisible(bool visible) override;

public:
  void setImage(QPixmap image);
  QPixmap getImage() const;

private:
  // GestureProcessor
  virtual void processPan(QPointF delta, QPointF global_point) override;
  virtual void processScaleChanged(qreal scale, QPointF center) override;
  virtual void
  processToucheEvent(QPointF delta,
                     const std::optional<QPointF> &touch_point) override;
  virtual void grabWidgetGesture(Qt::GestureType gesture) override;
  virtual void processDoubleTap(QPointF center) override;
  virtual void processAngleChanged(qreal rotation_delta,
                                   QPointF center) override;
  virtual void processSwipe(QSwipeGesture *) override;

protected:
  // QWidget
  virtual bool event(QEvent *event) override;
  virtual void paintEvent(QPaintEvent *) override;
  virtual void wheelEvent(QWheelEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

protected:
  // PhotoProcessor
  virtual double initialScaleFactor() const override;
  virtual void drawFrame(QPainter &painter) override;
  virtual QRectF scaleRefRect() const override;

private:
  void updatePhoto(const Core::PhotoDataPtr &photo);
  void updatePhoto(std::optional<QPointF> pos_delta,
                   std::optional<double> scale_factor,
                   std::optional<QPointF> center);
  void redrawBackgroundImage();

private:
  QPixmap image_;
  TouchButton *share_ = nullptr;
  TextButton *close_ = nullptr;
  QSvgRenderer *background_ = nullptr;
  QPixmap background_image_;
};

} // namespace FirstYear::UI::Preview
#endif // PREVIEW_WIDGET_H
