#include "PreviewWidget.h"
#include <stdafx.h>

namespace FirstYear::UI::Preview {
const double MIN_SIZE_K = 0.7;
const double MAX_SIZE_K = 15.0;
const double INITIAL_SCALE_FACTOR = 1.3;
const double ZOOM_STEP = 1.10;

void GestureProcessor::Initialise() {
  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;

  grabGestures(gestures);
}

void GestureProcessor::grabGestures(const QList<Qt::GestureType> &gestures) {
  for (Qt::GestureType gesture : gestures)
    grabWidgetGesture(gesture);
}

bool GestureProcessor::processEvent(QEvent *event) {

  switch (event->type()) {
  case QEvent::Gesture: {
    return gestureEvent(static_cast<QGestureEvent *>(event));
  }
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {

    toucheEvent(static_cast<QTouchEvent *>(event));
    return true;
  }
  default:
    return false;
  }
}

bool GestureProcessor::gestureEvent(QGestureEvent *event) {
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
  const auto touchPoints = touch->points();

  return processToucheEvent(touchPoints);
}

void GestureProcessor::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    is_gesture_moving_ = true;
    break;
  case Qt::GestureFinished:
  case Qt::GestureCanceled:
    is_gesture_moving_ = false;
    break;
  default:
    is_gesture_moving_ = false;
  }
#endif

  QPointF delta = gesture->delta();
  processPan(delta);
}

void GestureProcessor::pinchTriggered(QPinchGesture *gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    processScaleChanged(gesture->scaleFactor(), gesture->centerPoint());
  }
  if (gesture->state() == Qt::GestureFinished) {

    processScaleChanged(gesture->scaleFactor() / gesture->lastScaleFactor(),
                        gesture->centerPoint());
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
///
PhotoPainter::PhotoPainter()
    : dpr_(QGuiApplication::primaryScreen()->devicePixelRatio()) {}

void PhotoPainter::init(const Core::PhotoData &photo, QRectF destanation_rect,
                        QRectF boundary_rect) {

  photo_data_ = Core::PhotoData::CreateCopy(photo);
  boundary_rect_ = boundary_rect;
  destanation_rect_ = destanation_rect;

  const QRectF photo_rect = {
      QPointF(0, 0), photo_data_.image().size() /
                         QGuiApplication::primaryScreen()->devicePixelRatio()};
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = destanation_rect_.width() / destanation_rect_.height();

  if (k1 < k2) {
    internal_scale_ =
        ((double)destanation_rect_.height() / photo_rect.height()) *
        INITIAL_SCALE_FACTOR;
  } else {
    internal_scale_ = ((double)destanation_rect_.width() / photo_rect.width()) *
                      INITIAL_SCALE_FACTOR;
  }

  transform_ = getTransformForWidget(
      {std::optional<double>(), std::optional<QPointF>(),
       std::optional<QPointF>()},
      photo_data_.transformOffsetRef(), photo_data_.transformScaleRotateRef());
}

QTransform
PhotoPainter::getTransformForWidget(const PhotoPosition &photo_position,
                                    QTransform &transform_offset,
                                    QTransform &transform_scale_rotate) {
  // spdlog::info("a {}, s {},  ", photo_position_.angle, photo_.scale);
  const auto scale_diff = photo_position.scale.value_or(1);
  const auto offset_diff = photo_position.offset.value_or(QPointF(0, 0));
  const auto center = photo_position.center.value_or(QPointF(0, 0));

  const qreal iw = photo_data_.image().width() / dpr_;
  const qreal ih = photo_data_.image().height() / dpr_;
  const qreal wh = destanation_rect_.height();
  const qreal ww = destanation_rect_.width();

  const QPointF d = {destanation_rect_.left() + ww / 2,
                     destanation_rect_.top() + wh / 2};

  QTransform transform_internal1;
  transform_internal1.translate(d.x(), d.y());
  transform_internal1.scale(internal_scale_, internal_scale_);

  transform_offset.translate(offset_diff.x() / internal_scale_,
                             offset_diff.y() / internal_scale_);

  const QPointF c =
      (transform_scale_rotate * transform_offset * transform_internal1)
          .inverted()
          .map(center);

  transform_scale_rotate.translate(c.x(), c.y());
  transform_scale_rotate.scale(scale_diff, scale_diff);
  transform_scale_rotate.translate(-c.x(), -c.y());

  QTransform transform_internal2;
  transform_internal2.translate(-iw / 2, -ih / 2);

  return transform_internal2 * transform_scale_rotate * transform_offset *
         transform_internal1;
}

void PhotoPainter::drawPhoto(QPainter &painter) {
  if (photo_data_.image().isNull())
    return;

  const int frame_with = std::min(photo_data_.image().size().width(),
                                  photo_data_.image().size().height()) /
                         dpr_ / 50;
  const QSize frame_with_size{frame_with, frame_with};

  painter.setTransform(transform_);
  painter.setBrush(QColor(67, 56, 14, 20));
  painter.setPen(QPen(QColor(0, 0, 0, 0)));

  painter.drawRoundedRect(
      QRect{{frame_with, frame_with},
            photo_data_.image().size() / dpr_ + frame_with_size / 4},
      frame_with / 2, frame_with / 2);

  painter.setTransform(transform_);
  painter.drawPixmap(0, 0, photo_data_.image());
  painter.setTransform(QTransform());
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
///
bool PhotoProcessor::checkBoundares(const QTransform &transform) const {

  const QRectF image_rect = {
      QPointF(0, 0), photo_data_.image().size() /
                         QGuiApplication::primaryScreen()->devicePixelRatio()};

  auto translated_image_rect = transform.mapRect(image_rect);

  if (translated_image_rect.width() < boundary_rect_.width() * MIN_SIZE_K ||
      translated_image_rect.height() < boundary_rect_.height() * MIN_SIZE_K) {
    return false;
  }

  if (translated_image_rect.width() > boundary_rect_.width() * MAX_SIZE_K ||
      translated_image_rect.height() > boundary_rect_.height() * MAX_SIZE_K) {
    return false;
  }

  if (!translated_image_rect.intersects(boundary_rect_))
    return false;

  return true;
}

void PhotoProcessor::updatePhotoPosition(std::optional<QPointF> pos_delta,
                                         std::optional<double> scale_factor,
                                         std::optional<QPointF> center) {
  auto scale_rotate = photo_data_.transformScaleRotate();
  auto translate = photo_data_.transformOffset();
  auto transform = getTransformForWidget({scale_factor, pos_delta, center},
                                         translate, scale_rotate);

  if (checkBoundares(transform)) {
    transform_ = transform;
    photo_data_.setTransforms(std::move(scale_rotate), std::move(translate));
  }
}

/////////////////////////////////////////////////////////
/// \brief PhotoTuneWidget::PhotoTuneWidget
/// \param parent
///
///
PreviewWidget::PreviewWidget(QWidget &parent)
    : QWidget(&parent) /*, background_(c_background_str)*/ {
  GestureProcessor::Initialise();
  setAttribute(Qt::WA_AcceptTouchEvents);

  background_ = new QSvgRenderer(this);
  background_->load(QString(":/images/icons/rattles"));

  share_ = new ShareButton(this, true);
  connect(share_, &QPushButton::clicked, this,
          [&]() { emit SignalShareImage(); });

  close_ = new TextButton(this, true);
  close_->setText("Close");
  close_->setSize(QSize(110, 60));
  close_->setStyleSheet(c_close_button_style_str);
  connect(close_, &QPushButton::clicked, this, [&]() { hide(); });

  setGeometry(parent.geometry());
}

void PreviewWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  setAttribute(Qt::WA_AcceptTouchEvents, visible);
  if (!visible) {
    photo_data_ = Core::PhotoData::CreateEmptyData();
    emit SignalClosed();
  }
}

void PreviewWidget::redrawBackgroundImage() {
  background_image_ = QPixmap(size());
  QPainter painter(&background_image_);
  painter.fillRect(rect(), QColor(c_preview_background_color_str));

  const auto size = std::min(width(), height());
  background_->render(&painter, QRect{QPoint{}, QSize{size, size}});
}

void PreviewWidget::resizeEvent(QResizeEvent *e) {
  if (!e->size().isValid() || e->size().isEmpty()) {
    return;
  }
  QWidget::resizeEvent(e);

  redrawBackgroundImage();

  share_->setGeometry({{width() - share_->width() - height() / 40,
                        height() - share_->height() - height() / 20},
                       share_->size()});

  close_->setGeometry(
      {{width() - close_->width() - height() / 40, height() / 20},
       close_->size()});

  updatePhoto(photo_data_);
}

bool PreviewWidget::event(QEvent *event) {

  if (!isVisible() || !isEnabled()) {
    return QWidget::event(event);
  }

  if (GestureProcessor::processEvent(event)) {

    share_->event(event);
    close_->event(event);

    return true;
  }
  return QWidget::event(event);
}

void PreviewWidget::setImage(QPixmap photo) {

  Core::PhotoData photo_data = Core::PhotoData::CreateNewData(photo, false);

  updatePhoto(photo_data);
}

void PreviewWidget::updatePhoto(const Core::PhotoData &photo) {

  if (photo.image().isNull())
    return;

  auto boundary_rect = rect();
  boundary_rect.moveTopLeft(
      {boundary_rect.width() / 3, boundary_rect.height() / 3});
  boundary_rect.setHeight(boundary_rect.height() / 3);
  boundary_rect.setWidth(boundary_rect.width() / 3);

  PhotoProcessor::init(photo, rect(), boundary_rect);
  update();
}

QPixmap PreviewWidget::getImage() const { return photo_data_.image(); }

void PreviewWidget::updatePhoto(std::optional<QPointF> pos_delta,
                                std::optional<double> scale_factor,
                                std::optional<QPointF> center) {
  PhotoProcessor::updatePhotoPosition(pos_delta, scale_factor, center);
  update();
}

void PreviewWidget::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() < 0) {
    updatePhoto(std::optional<QPointF>(), ZOOM_STEP, event->position());
  } else if (event->angleDelta().y() > 0) {
    updatePhoto(std::optional<QPointF>(), 1.0 / ZOOM_STEP, event->position());
  }
}

bool PreviewWidget::processToucheEvent(const QList<QEventPoint> &points) {
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
    updatePhoto(delta, std::optional<double>(), std::optional<QPointF>());
    return true;
  }
  return false;
}

void PreviewWidget::processPan(QPointF delta) {
  updatePhoto(delta, std::optional<double>(), std::optional<QPointF>());
}

void PreviewWidget::processScaleChanged(qreal scale, QPointF center) {
  updatePhoto(std::optional<QPointF>(), scale, center);
}

void PreviewWidget::grabWidgetGesture(Qt::GestureType gesture) {
  QWidget::grabGesture(gesture);
}

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  hide();
}

void PreviewWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  painter.drawPixmap(rect(), background_image_, rect());

  PhotoProcessor::drawPhoto(painter);
}
} // namespace FirstYear::UI::Preview
