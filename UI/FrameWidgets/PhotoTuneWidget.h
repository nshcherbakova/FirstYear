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
class QTapGesture;
class QTouchEvent;

QT_END_NAMESPACE

namespace FirstYear::UI {

class PhotoTuneWidget final : public QWidget {
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
  bool gestureEvent(QGestureEvent *event);
  void panTriggered(QPanGesture *);
  void pinchTriggered(QPinchGesture *);
  void tapTriggered(QTapGesture *);
  bool toucheEvent(QTouchEvent *touch);

private:
  int id_ = 0;
  Core::PhotoData photo_;
  double internal_scale_ = 0;
  qreal currentStepScaleFactor = 1;

  bool is_gesture_moving_ = false;
  bool is_zooming_ = false;
  bool is_touch_movng_ = false;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
