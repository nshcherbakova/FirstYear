#include "PhotoTuneWidget.h"
#include "DefaultTemplateWidget.h"

#include <stdafx.h>
namespace FirstYear::UI {
const double MIN_SIZE_K = 0.7;
const double MAX_SIZE_K = 15.0;
const double INITIAL_SCALE_FACTOR = 2.5;
const double ZOOM_STEP = 1.10;
const double ROTATE_STEP = 0.5;

static const char *c_widget_background_str = ":/images/tune_photo/background";

void GestureProcessor::Initialise() {
  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  gestures << Qt::TapAndHoldGesture;
  // gestures << Qt::SwipeGesture;
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
  if (QGesture *tap_and_hold = event->gesture(Qt::TapAndHoldGesture))
    longTapTriggered(static_cast<QTapAndHoldGesture *>(tap_and_hold));
  if (QGesture *pan = event->gesture(Qt::PanGesture))
    panTriggered(static_cast<QPanGesture *>(pan));
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinchTriggered(static_cast<QPinchGesture *>(pinch));
  if (QGesture *pinch = event->gesture(Qt::SwipeGesture))
    swipeTriggered(static_cast<QSwipeGesture *>(pinch));

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
  if (changeFlags & QPinchGesture::RotationAngleChanged) {
    qreal rotation_delta =
        gesture->rotationAngle() - gesture->lastRotationAngle();
    processAngleChanged(rotation_delta, gesture->centerPoint());
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    processScaleChanged(gesture->scaleFactor(), gesture->centerPoint());
  }
  if (gesture->state() == Qt::GestureFinished) {

    processScaleChanged(gesture->scaleFactor() / gesture->lastScaleFactor(),
                        gesture->centerPoint());
  }
}

void GestureProcessor::longTapTriggered(QTapAndHoldGesture *gesture) {

  processLongTap(gesture);
}

void GestureProcessor::swipeTriggered(QSwipeGesture *gesture) {
  if (gesture->state() == Qt::GestureFinished) {
    processSwipe(gesture);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
void PhotoPainter::init(const Core::PhotoDataPtr &photo,
                        QRectF destanation_rect, QRectF boundary_rect) {

  photo_data_ = photo;
  boundary_rect_ = boundary_rect;
  destanation_rect_ = destanation_rect;

  const QRectF photo_rect = {
      QPointF(0, 0), photo_data_->image().size() /
                         QGuiApplication::primaryScreen()->devicePixelRatio()};

  const double k1 = photo_rect.width() / photo_rect.height();
  const double k2 = boundary_rect_.width() / boundary_rect_.height();

  if (k1 < k2) {
    internal_scale_ = ((double)boundary_rect_.height() / photo_rect.height()) *
                      INITIAL_SCALE_FACTOR;
  } else {
    internal_scale_ = ((double)boundary_rect_.width() / photo_rect.width()) *
                      INITIAL_SCALE_FACTOR;
  }

  transform_ = getTransformForWidget(
      {std::optional<double>(), std::optional<double>(),
       std::optional<QPointF>(), std::optional<QPointF>()},
      photo_data_->transformOffsetRef(),
      photo_data_->transformScaleRotateRef());
}

QTransform PhotoPainter::getTransformForWidget(
    const PhotoPosition &photo_position, Core::PhotoTransform &transform_offset,
    Core::PhotoTransform &transform_scale_rotate) {
  // spdlog::info("a {}, s {},  ", photo_position_.angle, photo_.scale);
  const auto angle_diff = photo_position.angle.value_or(0);
  const auto scale_diff = photo_position.scale.value_or(1);
  const auto offset_diff = photo_position.offset.value_or(QPointF(0, 0));
  const auto center = photo_position.center.value_or(QPointF(0, 0));

  const auto dpr = QGuiApplication::primaryScreen()->devicePixelRatio();

  const qreal iw = photo_data_->image().width() / dpr;
  const qreal ih = photo_data_->image().height() / dpr;
  const qreal wh = destanation_rect_.height();
  const qreal ww = destanation_rect_.width();

  QPointF d = {destanation_rect_.left() + ww / 2,
               destanation_rect_.top() + wh / 2};

  QTransform transform_internal1;
  transform_internal1.translate(d.x(), d.y());
  transform_internal1.scale(internal_scale_, internal_scale_);

  if (photo_position.offset.has_value()) {
    transform_offset.translate(offset_diff.x() / internal_scale_,
                               offset_diff.y() / internal_scale_);
  }

  if (photo_position.angle.has_value() || photo_position.scale.has_value() ||
      photo_position.center.has_value()) {

    QPointF c =
        (transform_scale_rotate * transform_offset * transform_internal1)
            .inverted()
            .map(center);

    transform_scale_rotate.translate(c.x(), c.y());
    transform_scale_rotate.rotate(angle_diff);
    transform_scale_rotate.scale(scale_diff, scale_diff);
    transform_scale_rotate.translate(-c.x(), -c.y());
  }

  QTransform transform_internal2;
  transform_internal2.translate(-iw / 2, -ih / 2);

  return transform_internal2 * transform_scale_rotate * transform_offset *
         transform_internal1;
}

void PhotoPainter::drawPhoto(QPainter &painter) {
  if (!photo_data_ || photo_data_->image().isNull())
    return;

  painter.setTransform(transform_);
  painter.drawPixmap(0, 0, photo_data_->image());
  painter.setTransform(QTransform());
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
///
bool PhotoProcessor::checkBoundares(const QTransform &transform) const {

  const QRectF image_rect = {
      QPointF(0, 0), photo_data_->image().size() /
                         QGuiApplication::primaryScreen()->devicePixelRatio()};

  const auto translated_image_rect = transform.mapRect(image_rect);

  if (translated_image_rect.width() < boundary_rect_.width() * MIN_SIZE_K ||
      translated_image_rect.height() < boundary_rect_.height() * MIN_SIZE_K)
    return false;

  if (translated_image_rect.width() > boundary_rect_.width() * MAX_SIZE_K ||
      translated_image_rect.height() > boundary_rect_.height() * MAX_SIZE_K)
    return false;

  if (!translated_image_rect.intersects(boundary_rect_))
    return false;

  return true;
}

void PhotoProcessor::updatePhotoPosition(std::optional<QPointF> pos_delta,
                                         std::optional<double> scale_factor,
                                         std::optional<double> angle_delta,
                                         std::optional<QPointF> center) {
  auto scale_rotate = photo_data_->transformScaleRotate();
  auto translate = photo_data_->transformOffset();
  auto transform = getTransformForWidget(
      {angle_delta, scale_factor, pos_delta, center}, translate, scale_rotate);

  if (checkBoundares(transform)) {
    transform_ = transform;
    photo_data_->setTransforms(std::move(scale_rotate), std::move(translate));
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
///

class TouchClickableLabel final : public ClickableLabel {
public:
  TouchClickableLabel(QWidget *parent, int font_size, QString font_color,
                      QString font_family)
      : ClickableLabel(parent, font_size, font_color, font_family) {
    setAttribute(Qt::WA_AcceptTouchEvents);
  }
  bool event(QEvent *event);
};

bool TouchClickableLabel::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd: {
    auto points = static_cast<QTouchEvent *>(event)->points();
    QPointF center;

    for (const QTouchEvent::TouchPoint &touchPoint : points) {
      center += touchPoint.position();
    }
    if (!points.empty()) {
      center /= points.size();
      if (geometry().contains(center.toPoint())) {
        return ClickableLabel::event(event);
      }
    }
    return false;
  }
  default:
    return ClickableLabel::event(event);
  }
  return ClickableLabel::event(event);
}
/////////////////////////////////////////////////////////
/// \brief PhotoTuneWidget::PhotoTuneWidget
/// \param parent
///
///
PhotoTuneWidget::PhotoTuneWidget(QWidget &parent)
    : QWidget(&parent), background_(c_widget_background_str) {
  GestureProcessor::Initialise();
  setAttribute(Qt::WA_AcceptTouchEvents);

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  open_file_ = new TextButton(this, true);
  open_file_->setText("Open");
  open_file_->setStyleSheet(c_open_button_style_str);
  connect(open_file_, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalOpenFile);

  next_ = new TextButton(this, true);
  next_->setText("Next");
  next_->setSize(QSize(120, 60));
  next_->setStyleSheet(c_dark_button_style_str);
  connect(next_, &QPushButton::clicked, this,
          [&]() { emit SignalTuneNextImage(); });

  prev_ = new TextButton(this, true);
  prev_->setText("Prev");
  prev_->setSize(QSize(120, 60));
  prev_->setStyleSheet(c_dark_button_style_str);
  connect(prev_, &QPushButton::clicked, this,
          [&]() { emit SignalTunePrevImage(); });

  text_ = new TouchClickableLabel(this, 10, "#408BB2", "Areal");
  text_->setAlignment(Qt::AlignCenter);
  connect(text_, &ClickableLabel::clicked, this,
          [&] { emit SignalTextClicked(text_->text()); });
}

void PhotoTuneWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  if (!visible && photo_data_) //&& !photo_data_->image().isNull())
  {
    emit SignalImageTuned();
  }
}
void PhotoTuneWidget::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);

  if (!photo_data_ || photo_data_->image().isNull())
    return;

  open_file_->setGeometry(
      {{width() - open_file_->width() - height() / 40, height() / 20},
       open_file_->size()});

  prev_->setGeometry(
      {{height() / 35, height() - height() / 10 - prev_->height()},
       prev_->size()});

  next_->setGeometry({{width() - next_->width() - height() / 35,
                       height() - height() / 10 - prev_->height()},
                      next_->size()});

  Frame::init(frame_data_, rect());

  updatePhoto(photo_data_);

  QRect rect = {width() / 5, (int)(height() / 20),
                width() - (int)(width() / 2.5), height() / 7};
  text_->setGeometry(rect);

  // QFont font = text_->fontS();
  //  font.setPointSize(width() / 20);
  text_->setFontSize(width() / 20);
}

bool PhotoTuneWidget::event(QEvent *event) {
  if (!isVisible() || !isEnabled()) {
    return QWidget::event(event);
  }

  if (GestureProcessor::processEvent(event)) {
    // close_->event(event);
    open_file_->event(event);
    next_->event(event);
    prev_->event(event);
    text_->event(event);
    return true;
  }
  return QWidget::event(event);
}

void PhotoTuneWidget::setPhoto(int id, const FrameParameters &frame_data,
                               const Core::PhotoDataPtr &photo, QString text) {

  id_ = id;
  Frame::init(frame_data, rect());
  updatePhoto(photo);
  text_->setText(text);
}

void PhotoTuneWidget::updatePhoto(const Core::PhotoDataPtr &photo) {
  PhotoProcessor::init(photo, rect(), frameRect());
  update();
}

void PhotoTuneWidget::updateText(QString text) { text_->setText(text); }

int PhotoTuneWidget::getPhotoId() const { return id_; }

Core::PhotoDataPtr PhotoTuneWidget::getPhoto() const { return photo_data_; }

void PhotoTuneWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  hide();
  // emit SignalImageTuned();
}

void PhotoTuneWidget::updatePhoto(std::optional<QPointF> pos_delta,
                                  std::optional<double> scale_factor,
                                  std::optional<double> angle_delta,
                                  std::optional<QPointF> center) {
  PhotoProcessor::updatePhotoPosition(pos_delta, scale_factor, angle_delta,
                                      center);
  update();
}

void PhotoTuneWidget::wheelEvent(QWheelEvent *event) {
  if (event->modifiers().testFlag(Qt::ControlModifier)) {

    if (event->angleDelta().y() < 0) {
      updatePhoto(std::optional<QPointF>(), std::optional<double>(),
                  ROTATE_STEP, event->position());
    } else if (event->angleDelta().y() > 0) {
      updatePhoto(std::optional<QPointF>(), std::optional<double>(),
                  -ROTATE_STEP, event->position());
    }
  } else if (event->angleDelta().y() < 0) {
    updatePhoto(std::optional<QPointF>(), ZOOM_STEP, std::optional<double>(),
                event->position());
  } else if (event->angleDelta().y() > 0) {
    updatePhoto(std::optional<QPointF>(), 1.0 / ZOOM_STEP,
                std::optional<double>(), event->position());
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
    updatePhoto(delta, std::optional<double>(), std::optional<double>(),
                std::optional<QPointF>());
    return true;
  }
  return false;
}

void PhotoTuneWidget::processSwipe(QSwipeGesture *gesture) {
  if (gesture->horizontalDirection() == QSwipeGesture::Right) {
    emit SignalTunePrevImage();
  }
}

void PhotoTuneWidget::processPan(QPointF delta) {
  updatePhoto(delta, std::optional<double>(), std::optional<double>(),
              std::optional<QPointF>());
}

void PhotoTuneWidget::processAngleChanged(qreal rotation_delta,
                                          QPointF center) {
  updatePhoto(std::optional<QPointF>(), std::optional<double>(), rotation_delta,
              center);
}

void PhotoTuneWidget::processScaleChanged(qreal scale, QPointF center) {
  updatePhoto(std::optional<QPointF>(), scale, std::optional<double>(), center);
}

void PhotoTuneWidget::processLongTap(QTapAndHoldGesture *) {

  // hide();
  // emit SignalImageTuned();
}

void PhotoTuneWidget::grabWidgetGesture(Qt::GestureType gesture) {
  QWidget::grabGesture(gesture);
}

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.drawPixmap(rect(), background_, rect());
  PhotoProcessor::drawPhoto(painter);
  Frame::drawFrame(painter);
}
} // namespace FirstYear::UI
