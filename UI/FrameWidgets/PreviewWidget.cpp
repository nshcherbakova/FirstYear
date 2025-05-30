#include "PreviewWidget.h"
#include <stdafx.h>

namespace FirstYear::UI::Preview {

const double INITIAL_SCALE_FACTOR = 1.3;
const double ZOOM_STEP = 1.10;
const double DOUBLE_TAP_SCALE_STEP = 1.4;

class PreviewPhotoData final : public Core::PhotoData {

public:
  explicit PreviewPhotoData(QPixmap image);

public:
  virtual const QPixmap &image() const override;
  virtual bool isStub() const override;

private:
  QPixmap image_;
};

PreviewPhotoData::PreviewPhotoData(QPixmap image)
    : Core::PhotoData(nullptr), image_(image) {}

const QPixmap &PreviewPhotoData::image() const { return image_; }

bool PreviewPhotoData::isStub() const { return false; }

PreviewWidget::PreviewWidget(QWidget &parent)
    : QWidget(&parent) /*, background_(c_background_str)*/ {

  setAttribute(Qt::WA_AcceptTouchEvents);

  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  gestures << DoubleTapGestureType;

  grabGestures(gestures);

  background_ = new QSvgRenderer(this);
  background_->load(QString(":/images/icons/rattles"));

  share_ = new ShareButton(this, true);
  connect(share_, &QPushButton::clicked, this,
          [&]() { emit SignalShareImage(); });

  close_ = new TextButton(this, true);
  close_->setText(tr("Back"));
  close_->setSize(QSize(110, 60));
  close_->setObjectName("Close");
  close_->setStyleSheet(c_close_button_style_str);
  connect(close_, &QPushButton::clicked, this, [&]() { hide(); });

  setGeometry(parent.geometry());
}

void PreviewWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  setAttribute(Qt::WA_AcceptTouchEvents, visible);
  if (!visible) {
    photo_data_.reset();
    emit SignalClosed();
  }
}

void PreviewWidget::redrawBackgroundImage() {
  background_image_ = QPixmap(size());
  QPainter painter(&background_image_);
  painter.fillRect(rect(), QColor("#e7e5ce"));

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
  updatePhoto(std::make_shared<PreviewPhotoData>(std::move(photo)));
}

void PreviewWidget::updatePhoto(const Core::PhotoDataPtr &photo) {

  if (!photo || photo->image().isNull())
    return;

  auto boundary_rect = rect();
  boundary_rect.moveTopLeft(
      {boundary_rect.width() / 3, boundary_rect.height() / 3});
  boundary_rect.setHeight(boundary_rect.height() / 3);
  boundary_rect.setWidth(boundary_rect.width() / 3);

  PhotoProcessor::init(photo, rect(), boundary_rect);
  update();
}

QPixmap PreviewWidget::getImage() const {
  UNI_ASSERT(photo_data_);
  return photo_data_->image();
}

void PreviewWidget::updatePhoto(std::optional<QPointF> pos_delta,
                                std::optional<double> scale_factor,
                                std::optional<QPointF> center) {
  PhotoProcessor::updatePhotoPosition(pos_delta, scale_factor, {}, center);
  update();
}

void PreviewWidget::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() < 0) {
    updatePhoto(std::optional<QPointF>(), ZOOM_STEP, event->position());
  } else if (event->angleDelta().y() > 0) {
    updatePhoto(std::optional<QPointF>(), 1.0 / ZOOM_STEP, event->position());
  }
}

void PreviewWidget::processToucheEvent(
    QPointF delta, const std::optional<QPointF> &touch_point) {

  updatePhoto(delta, std::optional<double>(), touch_point);
}

void PreviewWidget::processPan(QPointF delta, QPointF global_point) {
  updatePhoto(delta, std::optional<double>(), mapFromGlobal(global_point));
}

void PreviewWidget::processScaleChanged(qreal scale, QPointF center) {
  updatePhoto(std::optional<QPointF>(), scale, center);
}

void PreviewWidget::grabWidgetGesture(Qt::GestureType gesture) {
  QWidget::grabGesture(gesture);
}

void PreviewWidget::processAngleChanged(qreal, QPointF) {}

void PreviewWidget::processSwipe(QSwipeGesture *) {}

void PreviewWidget::processDoubleTap(QPointF center) {
  updatePhoto(std::optional<QPointF>(), DOUBLE_TAP_SCALE_STEP, center);
}

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);
  hide();
}

double PreviewWidget::initialScaleFactor() const {
  return INITIAL_SCALE_FACTOR;
}

void PreviewWidget::drawFrame(QPainter &painter) {
  UNI_ASSERT(photo_data_);

  const int frame_with = std::min(photo_data_->image().size().width(),
                                  photo_data_->image().size().height()) /
                         dpr_ / 50;
  const QSize frame_with_size{frame_with, frame_with};

  painter.setTransform(transform_);
  // shadow
  painter.setBrush(QColor(67, 56, 14, 20));
  painter.setPen(QPen(QColor(0, 0, 0, 0)));

  painter.drawRoundedRect(
      QRect{{frame_with, frame_with},
            photo_data_->image().size() / dpr_ + frame_with_size / 4},
      frame_with / 2, frame_with / 2);
}

QRectF PreviewWidget::scaleRefRect() const { return destanation_rect_; }

void PreviewWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  painter.drawPixmap(rect(), background_image_, rect());

  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  PhotoProcessor::drawPhoto(painter);
}
} // namespace FirstYear::UI::Preview
