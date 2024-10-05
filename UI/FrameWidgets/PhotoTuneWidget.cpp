#include "PhotoTuneWidget.h"

#include <stdafx.h>
namespace FirstYear::UI {
const double ZOOM_MIN = 0.1;
const double ZOOM_MAX = 10.0;
const double ZOOM_STEP = 1.10;
const double ROTATE_STEP = 0.5;
static const char *c_background_str = ":/images/tune_photo/background";

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
void PhotoPainter::init(const Core::PhotoData &photo, QRectF destanation_rect,
                        QRectF boundary_rect) {
  photo_ = photo;
  boundary_rect_ = boundary_rect;
  destanation_rect_ = destanation_rect;

  QRectF photo_rect = photo.image.rect();
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = boundary_rect_.width() / boundary_rect_.height();

  if (k1 < k2) {
    internal_scale_ = (double)boundary_rect_.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)boundary_rect_.width() / photo_rect.width();
  }
}

QTransform PhotoPainter::getTransformForWidget(QPointF offset, double scale,
                                               double angle) const {
  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = destanation_rect_.height();
  const qreal ww = destanation_rect_.width();

  QTransform transform;
  transform.translate(destanation_rect_.left() + ww / 2,
                      destanation_rect_.top() + wh / 2);
  transform.scale(internal_scale_, internal_scale_);
  transform.translate(offset.x(), offset.y());
  transform.rotate(angle);
  transform.scale(scale, scale);
  transform.translate(-iw / 2, -ih / 2);

  return transform;
}

void PhotoPainter::drawPhoto(QPainter &painter) {
  if (photo_.image.isNull())
    return;

  QTransform transform = getTransformForWidget(
      photo_.offset, currentStepScaleFactor() * photo_.scale, photo_.angle);
  painter.setTransform(transform);
  painter.drawPixmap(0, 0, photo_.image);
  painter.setTransform(QTransform());
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
///
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

void PhotoProcessor::updatePhotoPosition(QPointF pos_delta, double scale_factor,
                                         double angle_delta) {
  if (checkBoundares(photo_.offset + toImageCoordinates(pos_delta),
                     photo_.scale * scale_factor, photo_.angle + angle_delta)) {
    photo_.offset += toImageCoordinates(pos_delta);
    photo_.scale *= scale_factor;
    if (photo_.scale < ZOOM_MIN)
      photo_.scale = ZOOM_MIN;
    photo_.angle += angle_delta;

    spdlog::info("updatee position {}", photo_.scale);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///

void Frame::init(const FrameParameters &frame_data, QRectF widget_rect) {

  frame_data_ = frame_data;

  if (frame_data_.type == FrameParameters::TYPE::RECT ||
      frame_data_.type == FrameParameters::TYPE::ROUND) {

    auto frame_size = frame_data_.data.toSizeF().scaled(
        widget_rect.width() / 2, widget_rect.height(), Qt::KeepAspectRatio);

    frame_boundary_rect_.setTopLeft(QPoint(0, 0));
    frame_boundary_rect_.setSize(frame_size);
    const auto half_diff = (widget_rect.size() - frame_size) / 2.0;
    QPoint upper_left_corner(half_diff.width(), half_diff.height());
    frame_boundary_rect_.translate(upper_left_corner);
  }
}

void Frame::drawFrame(QPainter &painter) {
  if (frame_data_.type == FrameParameters::TYPE::RECT) {
    painter.setPen(Qt::gray);
    painter.drawRoundedRect(frame_boundary_rect_, 10, 10);
    QRectF new_rect(frame_boundary_rect_);
    painter.setPen(Qt::white);
    new_rect.moveTopLeft(frame_boundary_rect_.topLeft() + QPoint{1, 1});
    painter.drawRoundedRect(new_rect, 10, 10);
  } else

      if (frame_data_.type == FrameParameters::TYPE::ROUND) {
    painter.setPen(Qt::gray);
    painter.drawEllipse(frame_boundary_rect_);
    QRectF new_rect(frame_boundary_rect_);
    painter.setPen(Qt::white);
    new_rect.moveTopLeft(frame_boundary_rect_.topLeft() + QPoint{1, 1});
    painter.drawEllipse(new_rect);
  }
}

QRectF Frame::frameRect() { return frame_boundary_rect_; }

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
PhotoTuneWidget::PhotoTuneWidget(QWidget &parent)
    : QWidget(&parent), background_(c_background_str) {
  GestureProcessor::Initialise();
  setAttribute(Qt::WA_AcceptTouchEvents);

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  // setContentsMargins(10, 10, 10, 10);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  int button_with = 40;
  int button_margin = 20;

  auto open_file = new QPushButton(this);
  open_file->setGeometry(button_margin, geometry().height() - 2 * button_with,
                         2 * button_with, button_with);
  open_file->setText("Open");
  open_file->setContentsMargins(0, 0, 0, 0);
  connect(open_file, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalOpenFile);

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

void PhotoTuneWidget::setPhoto(int id, const FrameParameters &frame_data,
                               const Core::PhotoData &photo) {

  id_ = id;
  Frame::init(frame_data, rect());
  PhotoProcessor::init(photo, rect(), frameRect());
  update();
}

void PhotoTuneWidget::updatePhoto(const Core::PhotoData &photo) {
  PhotoProcessor::init(photo, rect(), frameRect());
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

void PhotoTuneWidget::wheelEvent(QWheelEvent *event) {
  if (event->modifiers().testFlag(Qt::ControlModifier)) {

    if (event->angleDelta().y() < 0) {
      updatePhoto(QPointF(), 1, ROTATE_STEP);
    } else if (event->angleDelta().y() > 0) {
      this->updatePhoto(QPointF(), 1, -ROTATE_STEP);
    }
  } else if (event->angleDelta().y() < 0 && this->photo_.scale < ZOOM_MAX) {
    updatePhoto(QPointF(), ZOOM_STEP, 0);
  } else if (event->angleDelta().y() > 0 && this->photo_.scale >= ZOOM_MIN) {
    this->updatePhoto(QPointF(), 1.0 / ZOOM_STEP, 0);
  }
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

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.drawPixmap(rect(), background_, rect());
  PhotoProcessor::drawPhoto(painter);
  Frame::drawFrame(painter);
}
} // namespace FirstYear::UI
