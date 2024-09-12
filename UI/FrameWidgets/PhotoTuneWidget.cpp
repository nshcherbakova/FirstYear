#include "PhotoTuneWidget.h"

#include <stdafx.h>
namespace FirstYear::UI {

PhotoTuneWidget::PhotoTuneWidget(QWidget &parent) : QWidget(&parent) {

  setAttribute(Qt::WA_AcceptTouchEvents);

  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  gestures << Qt::TapAndHoldGesture;
  grabGestures(gestures);

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  // setContentsMargins(10, 10, 10, 10);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  double zoom_step = 1.3;
  double ZOOM_MIN = 0.1;
  double ZOOM_MAX = 10.0;

  double move_step = 10;

  double rotate_step = 0.5;

  int button_with = 40;
  int button_margin = 20;
  int button_space = 10;
  auto zoom_in = new QPushButton(this);
  zoom_in->setGeometry(button_margin, button_margin, button_with, button_with);
  zoom_in->setText("+");
  zoom_in->setContentsMargins(0, 0, 0, 0);
  connect(zoom_in, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MAX]() {
    if (this->photo_.scale < ZOOM_MAX) {
      updatePhotoPosition(QPointF(), zoom_step, 0);

      this->update();
    }
  });

  auto zoom_out = new QPushButton(this);
  zoom_out->setGeometry(button_margin,
                        button_margin + button_with + button_space, button_with,
                        button_with);
  zoom_out->setText("-");
  zoom_out->setContentsMargins(0, 0, 0, 0);
  connect(zoom_out, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MIN]() {
    if (this->photo_.scale >= ZOOM_MIN) {
      this->updatePhotoPosition(QPointF(), 1.0 / zoom_step, 0);
      this->update();
    }
  });

  auto rotate = new QPushButton(this);
  rotate->setGeometry(geometry().width() - button_margin - button_with,
                      button_margin, button_with, button_with);
  rotate->setText("r");
  rotate->setContentsMargins(0, 0, 0, 0);
  connect(rotate, &QPushButton::clicked, this, [&, rotate_step]() {
    {
      this->updatePhotoPosition(QPointF(), 1, rotate_step);
      this->update();
    }
  });

  auto open_file = new QPushButton(this);
  open_file->setGeometry(button_margin, geometry().height() - 2 * button_with,
                         2 * button_with, button_with);
  open_file->setText("Open");
  open_file->setContentsMargins(0, 0, 0, 0);
  connect(open_file, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalPhotoChanged);

  auto close = new QPushButton(this);
  close->setAttribute(Qt::WA_AcceptTouchEvents, true);
  close->setGeometry(geometry().width() - 3 * button_with,
                     geometry().height() - 2 * button_with, 2 * button_with,
                     button_with);
  close->setText("Close");
  close->setContentsMargins(0, 0, 0, 0);
  connect(close, &QPushButton::clicked, this, [&]() {
    hide();
    emit SignalImageTuned();
  });
}

void PhotoTuneWidget::setPhoto(int id, const Core::PhotoData &photo) {
  id_ = id;
  photo_ = photo;

  QRectF photo_rect = photo.image.rect();
  QRectF widget_rect = rect();
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = widget_rect.width() / widget_rect.height();

  internal_scale_ = 1;

  if (k1 < k2) {
    internal_scale_ = (double)widget_rect.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)widget_rect.width() / photo_rect.width();
  }

  update();
}

bool PhotoTuneWidget::checkBoundares(QPointF offset, double scale,
                                     double angle) {
  QRectF image_rect = photo_.image.rect();
  QRectF frame_rect = rect();

  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = height();
  const qreal ww = width();

  QTransform transform;
  transform.translate(ww / 2, wh / 2);
  transform.scale(internal_scale_, internal_scale_);
  transform.translate(offset.x(), offset.y());
  transform.rotate(angle);
  transform.scale(scale * currentStepScaleFactor,
                  scale * currentStepScaleFactor);
  transform.translate(-iw / 2, -ih / 2);

  auto translated_image_polygon = transform.mapToPolygon(image_rect.toRect());
  QPolygon frame_polygon(frame_rect.toRect());
  for (int i = 0; i < frame_polygon.size(); i++) {
    if (!translated_image_polygon.containsPoint(frame_polygon.point(i),
                                                Qt::OddEvenFill))
      return false;
  }

  return true;
}

QPointF PhotoTuneWidget::toImageCoordinates(QPointF point) {
  return point / internal_scale_;
}

/*QRectF PhotoTuneWidget::toInternalCoordinates(QRectF rect)
{
    return rect. * internal_scale_;
}*/

int PhotoTuneWidget::getPhotoId() const { return id_; }
Core::PhotoData PhotoTuneWidget::getPhoto() const { return photo_; }

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (photo_.image.isNull())
    return;

  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = height();
  const qreal ww = width();

  QTransform transform;
  transform.translate(ww / 2, wh / 2);
  transform.scale(internal_scale_, internal_scale_);
  transform.translate(photo_.offset.x(), photo_.offset.y());
  transform.rotate(photo_.angle);
  transform.scale(currentStepScaleFactor * photo_.scale,
                  currentStepScaleFactor * photo_.scale);
  transform.translate(-iw / 2, -ih / 2);

  /*
    painter.translate(ww / 2, wh / 2);
    painter.scale(internal_scale_, internal_scale_);
    painter.translate(photo_.offset.x(), photo_.offset.y());
  painter.rotate(photo_.angle);
    painter.scale(currentStepScaleFactor * photo_.scale,
                  currentStepScaleFactor * photo_.scale);
    painter.translate(-iw / 2, -ih / 2);
  */
  painter.setTransform(transform);
  painter.drawPixmap(0, 0, photo_.image);
}

void PhotoTuneWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  hide();
  emit SignalImageTuned();
}

Q_LOGGING_CATEGORY(lcExample, "qt.examples.imagegestures")

void PhotoTuneWidget::grabGestures(const QList<Qt::GestureType> &gestures) {
  //! [enable gestures]
  for (Qt::GestureType gesture : gestures)
    grabGesture(gesture);
  //! [enable gestures]
}

//! [event handler]
bool PhotoTuneWidget::event(QEvent *event) {

  switch (event->type()) {
  case QEvent::Gesture: {
    // event->accept();
    return gestureEvent(static_cast<QGestureEvent *>(event));
  }
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {

    bool result = toucheEvent(static_cast<QTouchEvent *>(event));

    if (!result)
      return QWidget::event(event);
    // for(auto& child: children())
    {
      //  if( qobject_cast<QAbstractButton *>(child))
      //      if( child->event(event) && event->isAccepted())
      //          return true;
    }
    return true;
  }
  default:
    return QWidget::event(event);
  }
}
bool PhotoTuneWidget::toucheEvent(QTouchEvent *touch) {

  if (is_zooming_ || is_gesture_moving_) {
    return false;
  }
  touch->accept();
  spdlog::info("QTouchEvent {}", (long)touch);
  const auto touchPoints = touch->points();
  QPointF delta;
  int count = 0;

  for (const QTouchEvent::TouchPoint &touchPoint : touchPoints) {
    switch (touchPoint.state()) {
    case QEventPoint::Stationary:
    case QEventPoint::Released:
      // don't do anything if this touch point hasn't moved or has been released
      continue;
    default: {

      delta += touchPoint.position() - touchPoint.lastPosition();
    }
    }
    count += 1;
  }
  if (count > 0) {
    delta /= count;
    updatePhotoPosition(delta, 1, 0);

    return true;
  }
  return false;
}

void PhotoTuneWidget::updatePhotoPosition(QPointF pos_delta,
                                          double scale_factor,
                                          double angle_delta) {
  if (checkBoundares(photo_.offset + toImageCoordinates(pos_delta),
                     photo_.scale * scale_factor, photo_.angle + angle_delta)) {
    photo_.offset += toImageCoordinates(pos_delta);
    photo_.scale *= scale_factor;
    photo_.angle += angle_delta;
    update();
  }
}
//! [event handler]
//!
//! [gesture event handler]
bool PhotoTuneWidget::gestureEvent(QGestureEvent *event) {
  spdlog::info("gestureEvent {}", (long)event);
  if (QGesture *tap_and_hold = event->gesture(Qt::TapAndHoldGesture))
    tapTriggered(static_cast<QTapAndHoldGesture *>(tap_and_hold));
  if (QGesture *pan = event->gesture(Qt::PanGesture))
    panTriggered(static_cast<QPanGesture *>(pan));
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinchTriggered(static_cast<QPinchGesture *>(pinch));

  return true;
}
//! [gesture event handler]

void PhotoTuneWidget::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    setCursor(Qt::SizeAllCursor);
    is_gesture_moving_ = true;
    break;
  case Qt::GestureFinished:
  case Qt::GestureCanceled:
    is_gesture_moving_ = false;
    break;
  default:
    setCursor(Qt::ArrowCursor);
    is_gesture_moving_ = false;
  }
#endif
  QPointF delta = gesture->delta();
  spdlog::info("panTriggered ");
  updatePhotoPosition(delta, 1, 0);

  // update();
}

void PhotoTuneWidget::tapTriggered(QTapAndHoldGesture *gesture) {
  /*
#ifndef QT_NO_CURSOR
switch (gesture->state()) {
case Qt::GestureStarted:
case Qt::GestureUpdated:
  setCursor(Qt::SizeAllCursor);
  is_zooming_ = true;
  break;
case Qt::GestureFinished:
case Qt::GestureCanceled:
  is_zooming_ = false;
  break;
default:
  setCursor(Qt::ArrowCursor);
  is_zooming_ = false;
}
#endif
QPointF position = gesture->position();
spdlog::info("tapTriggered ");
updatePhotoPosition(position, 1,0);*/
  hide();
  emit SignalImageTuned();
  // update();
}

//! [pinch function]
void PhotoTuneWidget::pinchTriggered(QPinchGesture *gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::RotationAngleChanged) {
    qreal rotationDelta =
        gesture->rotationAngle() - gesture->lastRotationAngle();

    updatePhotoPosition(QPointF(), 1, rotationDelta);
    qCDebug(lcExample) << "pinchTriggered(): rotate by" << rotationDelta << "->"
                       << photo_.angle;
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    currentStepScaleFactor = gesture->totalScaleFactor();
    qCDebug(lcExample) << "pinchTriggered(): zoom by" << gesture->scaleFactor()
                       << "->" << currentStepScaleFactor;
  }
  if (gesture->state() == Qt::GestureFinished) {

    updatePhotoPosition(QPointF(), currentStepScaleFactor, 0);
    currentStepScaleFactor = 1;
  }
  // update();
}

} // namespace FirstYear::UI
