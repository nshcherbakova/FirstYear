#include "PhotoTuneWidget.h"
#include "DefaultTemplateWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {
const double INITIAL_SCALE_FACTOR = 2.5;
const double ZOOM_STEP = 1.10;
const double ROTATE_STEP = 0.5;
const double DOUBLE_TAP_SCALE_STEP = 1.4;

void Frame::init(const FrameParameters &frame_data, QRectF widget_rect) {

  if (frame_data.data.isNull()) {
    return;
  }

  frame_data_ = frame_data;

  if (frame_data_.type == FrameParameters::TYPE::RECT ||
      frame_data_.type == FrameParameters::TYPE::ROUND) {

    // auto size = std::min(widget_rect.width(), widget_rect.height());
    auto frame_size = frame_data_.data.toSizeF().scaled(
        widget_rect.width() / 1.5, widget_rect.height() / 1.5,
        Qt::KeepAspectRatio);

    frame_boundary_rect_.setTopLeft(QPoint(0, 0));
    frame_boundary_rect_.setSize(frame_size);
    const auto half_diff = (widget_rect.size() - frame_size) / 2.0;
    QPoint upper_left_corner(half_diff.width(), half_diff.height());
    frame_boundary_rect_.translate(upper_left_corner);
  }
}

void Frame::drawFrame(QPainter &painter) {
  painter.setRenderHints(QPainter::RenderHint::Antialiasing);

  QColor dark_color(c_tune_frame_dark_color_str);
  QPen pen_dark(dark_color);
  pen_dark.setWidth(3);

  QColor light_color(c_tune_frame_light_color_str);
  QPen pen_light(light_color);
  pen_light.setWidth(3);

  painter.setPen(pen_dark);

  QRectF rect(frame_boundary_rect_);
  rect.moveTopLeft(frame_boundary_rect_.topLeft() + QPoint{-4, -4});
  if (frame_data_.type == FrameParameters::TYPE::RECT) {
    painter.drawRoundedRect(rect, 10, 10);
  } else {
    painter.drawEllipse(rect);
  }

  painter.setPen(pen_light);

  QRectF new_rect(frame_boundary_rect_);
  new_rect.moveTopLeft(frame_boundary_rect_.topLeft() + QPoint{4, 4});

  if (frame_data_.type == FrameParameters::TYPE::RECT) {
    painter.drawRoundedRect(new_rect, 10, 10);
  } else {
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
    setStyleSheet(c_clickable_label_style_str);
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

PhotoTuneWidget::PhotoTuneWidget(QWidget &parent)
    : QWidget(&parent),
      dpr_(QGuiApplication::primaryScreen()->devicePixelRatio()) {

  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  gestures << DoubleTapGestureType;

  grabGestures(gestures);

  background_ = new QSvgRenderer(this);
  background_->load(QString(":/images/icons/stars2"));

  open_file_ = new TextButton(this, true);
  open_file_->setIcon(QIcon(":/images/icons/open_button"));
  open_file_->setIconSize(QSize(50, 50));
  open_file_->setSize(QSize(80, 60));
  open_file_->setStyleSheet(c_open_image_button_style_str);
  connect(open_file_, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalOpenFile);

  close_ = new TextButton(this, true);
  close_->setText("Close");
  close_->setSize(QSize(110, 60));
  close_->setStyleSheet(c_open_button_style_str);
  connect(close_, &QPushButton::clicked, this, [&]() { hide(); });

  next_ = new TextButton(this, true);
  next_->setIcon(QIcon(":/images/icons/next"));
  next_->setIconSize(QSize(40, 40));
  next_->setSize(QSize(80, 60));
  next_->setStyleSheet(c_dark_button_style_str);
  connect(next_, &QPushButton::clicked, this,
          [&]() { emit SignalTuneNextImage(); });

  prev_ = new TextButton(this, true);
  prev_->setIcon(QIcon(":/images/icons/prev"));
  prev_->setIconSize(QSize(40, 40));
  prev_->setSize(QSize(80, 60));
  prev_->setStyleSheet(c_dark_button_style_str);
  connect(prev_, &QPushButton::clicked, this,
          [&]() { emit SignalTunePrevImage(); });

  text_ =
      new TouchClickableLabel(this, 10, c_tune_frame_text_color_str, "Areal");
  text_->setAlignment(Qt::AlignCenter);
  connect(text_, &ClickableLabel::clicked, this,
          [&] { emit SignalTextClicked(text_->text()); });
  resizeEvent(nullptr);
}

void PhotoTuneWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  setAttribute(Qt::WA_AcceptTouchEvents, visible);
  if (!visible && photo_data_) //&& !photo_data_->image().isNull())
  {
    emit SignalImageTuned();
  }
}

void PhotoTuneWidget::redrawBackgroundImage() {

  background_image_ = QPixmap(size() * dpr_);
  background_image_.setDevicePixelRatio(dpr_);

  QPainter painter(&background_image_);

  painter.fillRect(rect(), Qt::white);
  const auto size = std::min(width(), height()) * 2;
  background_->render(&painter, QRect{QPoint{(width() - size) / 2,
                                             (int)((height() - size) / 1.8)},
                                      QSize{size, size}});
}

void PhotoTuneWidget::resizeEvent(QResizeEvent *e) {
  if (e) {
    if (!e->size().isValid() || e->size().isEmpty()) {
      return;
    }
    QWidget::resizeEvent(e);
  }

  redrawBackgroundImage();

  open_file_->setGeometry({{height() / 40, height() / 20}, open_file_->size()});

  close_->setGeometry(
      {{width() - close_->width() - height() / 40, height() / 20},
       close_->size()});

  prev_->setGeometry(
      {{height() / 35, height() - height() / 20 - prev_->height()},
       prev_->size()});

  next_->setGeometry({{width() - next_->width() - height() / 35,
                       height() - height() / 20 - prev_->height()},
                      next_->size()});

  if (photo_data_ && !photo_data_->image().isNull()) {
    Frame::init(frame_data_, rect());
    updatePhoto(photo_data_);
  }

  const auto top = std::max((int)frameRect().top() - 70, 0);
  const auto text_width = (int)(width() / 1.5);
  QRect rect(QPoint{(int)(width() - text_width) / 2, top},
             QSize{text_width, 40});
  text_->setGeometry(rect);
  text_->setFontSize(30);
}

bool PhotoTuneWidget::event(QEvent *event) {
  if (!isVisible() || !isEnabled()) {
    return QWidget::event(event);
  }

  if (GestureProcessor::processEvent(event)) {
    open_file_->event(event);
    close_->event(event);
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
  resizeEvent(nullptr);
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

void PhotoTuneWidget::processDoubleTap(QPointF center) {
  updatePhoto(std::optional<QPointF>(), DOUBLE_TAP_SCALE_STEP,
              std::optional<double>(), center);
}

void PhotoTuneWidget::grabWidgetGesture(Qt::GestureType gesture) {
  QWidget::grabGesture(gesture);
}

double PhotoTuneWidget::initialScaleFactor() const {
  return INITIAL_SCALE_FACTOR;
}

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  painter.drawPixmap(rect(), background_image_,
                     QRect{{0, 0}, rect().size() * dpr_});

  PhotoProcessor::drawPhoto(painter);
  Frame::drawFrame(painter);
}
} // namespace FirstYear::UI
