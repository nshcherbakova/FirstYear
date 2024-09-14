#include "PhotoTuneWidget.h"

#include <stdafx.h>
namespace FirstYear::UI {

void GestureProcessor::Initialise() {
  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  gestures << Qt::TapAndHoldGesture;
  grabGestures(gestures);
}

void GestureProcessor::grabGestures(const QList<Qt::GestureType> &gestures) {
  for (Qt::GestureType gesture : gestures)
    grabWidgetGesture(gesture);
}

bool GestureProcessor::processEvent(QEvent *event) {

  switch (event->type()) {
  case QEvent::Gesture: {
    // event->accept();
    return gestureEvent(static_cast<QGestureEvent *>(event));
  }
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {

    toucheEvent(static_cast<QTouchEvent *>(event));

    // if (!result)
    //     return QWidget::event(event);
    // for(auto& child: children())
    {
      //  if( qobject_cast<QAbstractButton *>(child))
      //      if( child->event(event) && event->isAccepted())
      //          return true;
    }
    return true;
  }
  default:
    return false;
  }
}

bool GestureProcessor::gestureEvent(QGestureEvent *event) {
  spdlog::info("gestureEvent {}", (long)event);
  if (QGesture *tap_and_hold = event->gesture(Qt::TapAndHoldGesture))
    longTapTriggered(static_cast<QTapAndHoldGesture *>(tap_and_hold));
  if (QGesture *pan = event->gesture(Qt::PanGesture))
    panTriggered(static_cast<QPanGesture *>(pan));
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinchTriggered(static_cast<QPinchGesture *>(pinch));

  return true;
}

bool GestureProcessor::toucheEvent(QTouchEvent *touch) {

  if (is_zooming_ || is_gesture_moving_) {
    return false;
  }
  touch->accept();
  spdlog::info("QTouchEvent {}", (long)touch);
  const auto touchPoints = touch->points();

  return processToucheEvent(touchPoints);
}

void GestureProcessor::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    // setCursor(Qt::SizeAllCursor);
    is_gesture_moving_ = true;
    break;
  case Qt::GestureFinished:
  case Qt::GestureCanceled:
    is_gesture_moving_ = false;
    break;
  default:
    // setCursor(Qt::ArrowCursor);
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

    processAngleChanged(rotation_delta);
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    currentStepScaleFactor_ = gesture->totalScaleFactor();
  }
  if (gesture->state() == Qt::GestureFinished) {

    processScaleChanged(currentStepScaleFactor_);

    currentStepScaleFactor_ = 1;
  }
}

void GestureProcessor::longTapTriggered(QTapAndHoldGesture *gesture) {

  processLongTap(gesture);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///

void PhotoProcessor::init(const Core::PhotoData &photo, QRectF boundary_rect) {
  photo_ = photo;
  boundary_rect_ = boundary_rect;

  QRectF photo_rect = photo.image.rect();
  QRectF widget_rect = boundary_rect_;
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = widget_rect.width() / widget_rect.height();

  internal_scale_ = 1;

  if (k1 < k2) {
    internal_scale_ = (double)widget_rect.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)widget_rect.width() / photo_rect.width();
  }
}

bool PhotoProcessor::checkBoundares(QPointF offset, double scale,
                                    double angle) const {

  QTransform transform =
      getTransformForWidget(offset, scale * currentStepScaleFactor(), angle);

  QRectF image_rect = photo_.image.rect();

  auto translated_image_rect = transform.mapRect(image_rect);

  if (translated_image_rect.intersects(boundary_rect_))
    return true;

  return false;
}

QPointF PhotoProcessor::toImageCoordinates(QPointF point) const {
  return point / internal_scale_;
}

QTransform PhotoProcessor::getTransformForWidget(QPointF offset, double scale,
                                                 double angle) const {
  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = widgetRect().height();
  const qreal ww = widgetRect().width();

  QTransform transform;
  transform.translate(widgetRect().left() + ww / 2,
                      widgetRect().top() + wh / 2);
  transform.scale(internal_scale_, internal_scale_);
  transform.translate(offset.x(), offset.y());
  transform.rotate(angle);
  transform.scale(scale, scale);
  transform.translate(-iw / 2, -ih / 2);

  return transform;
}

void PhotoProcessor::drawPhoto(QPainter &painter) {
  if (photo_.image.isNull())
    return;

  QTransform transform = getTransformForWidget(
      photo_.offset, currentStepScaleFactor() * photo_.scale, photo_.angle);

  painter.setTransform(transform);
  painter.drawPixmap(0, 0, photo_.image);
  painter.setTransform(QTransform());

  spdlog::info("drawPhoto ");
}

void PhotoProcessor::updatePhotoPosition(QPointF pos_delta, double scale_factor,
                                         double angle_delta) {
  if (checkBoundares(photo_.offset + toImageCoordinates(pos_delta),
                     photo_.scale * scale_factor, photo_.angle + angle_delta)) {
    photo_.offset += toImageCoordinates(pos_delta);
    photo_.scale *= scale_factor;
    photo_.angle += angle_delta;

    spdlog::info("updatee position ");
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///

void Frame::init(QSizeF frame_size, QRectF widget_rect) {

  frame_.setTopLeft(QPoint(0, 0));
  frame_.setSize(frame_size);
  const auto half_diff = (widget_rect.size() - frame_size) / 2.0;
  QPoint upper_left_corner(half_diff.width(), half_diff.height());
  frame_.translate(upper_left_corner);
}

void Frame::drawFrame(QPainter &painter) {
  painter.setPen(Qt::white);
  painter.drawRoundedRect(frame_, 10, 10);
}

QRectF Frame::frameRect() { return frame_; }

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
PhotoTuneWidget::PhotoTuneWidget(QWidget &parent) : QWidget(&parent) {
  GestureProcessor::Initialise();
  setAttribute(Qt::WA_AcceptTouchEvents);

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  // setContentsMargins(10, 10, 10, 10);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  double zoom_step = 1.3;
  double ZOOM_MIN = 0.1;
  double ZOOM_MAX = 10.0;

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
      updatePhoto(QPointF(), zoom_step, 0);
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
      this->updatePhoto(QPointF(), 1.0 / zoom_step, 0);
    }
  });

  auto rotate = new QPushButton(this);
  rotate->setGeometry(geometry().width() - button_margin - button_with,
                      button_margin, button_with, button_with);
  rotate->setText("r");
  rotate->setContentsMargins(0, 0, 0, 0);
  connect(rotate, &QPushButton::clicked, this, [&, rotate_step]() {
    { this->updatePhoto(QPointF(), 1, rotate_step); }
  });

  auto open_file = new QPushButton(this);
  open_file->setGeometry(button_margin, geometry().height() - 2 * button_with,
                         2 * button_with, button_with);
  open_file->setText("Open");
  open_file->setContentsMargins(0, 0, 0, 0);
  connect(open_file, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalPhotoChanged);

  auto close = new QPushButton(this);
  // close->setAttribute(Qt::WA_AcceptTouchEvents, true);
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

bool PhotoTuneWidget::event(QEvent *event) {

  if (GestureProcessor::processEvent(event)) {
    return true;
  }
  return QWidget::event(event);
}

void PhotoTuneWidget::setPhoto(int id, QSizeF frame_size,
                               const Core::PhotoData &photo) {

  id_ = id;

  Frame::init(frame_size, rect());
  PhotoProcessor::init(photo, frameRect());
  update();
}

int PhotoTuneWidget::getPhotoId() const { return id_; }
Core::PhotoData PhotoTuneWidget::getPhoto() const { return photo_; }

void PhotoTuneWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  hide();
  emit SignalImageTuned();
}

void PhotoTuneWidget::updatePhoto(QPointF pos_delta, double scale_factor,
                                  double angle_delta) {
  PhotoProcessor::updatePhotoPosition(pos_delta, scale_factor, angle_delta);
  update();
}

bool PhotoTuneWidget::processToucheEvent(const QList<QEventPoint> &points) {
  QPointF delta;
  int count = 0;

  for (const QTouchEvent::TouchPoint &touchPoint : points) {
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
    updatePhoto(delta, 1, 0);
    return true;
  }
  return false;
}

void PhotoTuneWidget::processPan(QPointF delta) {
  spdlog::info("panTriggered ");
  updatePhoto(delta, 1, 0);
}

void PhotoTuneWidget::processAngleChanged(qreal rotation_delta) {
  updatePhoto(QPointF(), 1, rotation_delta);
}

void PhotoTuneWidget::processScaleChanged(qreal scale) {
  updatePhoto(QPointF(), scale, 0);
}

void PhotoTuneWidget::processLongTap(QTapAndHoldGesture *) {

  hide();
  emit SignalImageTuned();
}

void PhotoTuneWidget::grabWidgetGesture(Qt::GestureType gesture) {
  QWidget::grabGesture(gesture);
}

double PhotoTuneWidget::currentStepScaleFactor() const {
  return currentStepScaleFactor_;
}

QRectF PhotoTuneWidget::widgetRect() const { return rect(); }

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  PhotoProcessor::drawPhoto(painter);
  Frame::drawFrame(painter);
}
} // namespace FirstYear::UI
