#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Core::PhotoDataPtr &photo,
                           const QColor &background) {
  photo_data_ = photo;
  background_ = background;
  PhotoPainter::init(photo, rect(), rect());

  UpdateButton();
}

void ImageButton::UpdateButton() {
  if (buffer_.size() != rect().size()) {
    buffer_ = QPixmap(rect().size() * devicePixelRatio());
    buffer_.setDevicePixelRatio(devicePixelRatio());
  }
  {
    QPainter painter(&buffer_);

    const auto rect = QRect({0, 0}, buffer_.size());

    painter.fillRect(rect, background_);

    PhotoPainter::drawPhoto(painter);
  }
  OnUpdateImageBuffer(buffer_);
  update();
}

void ImageButton::resizeEvent(QResizeEvent *e) {
  QPushButton::resizeEvent(e);
  UpdateButton();
}

void ImageButton::paintEvent(QPaintEvent *) {

  QPainter painter(this);
  painter.drawPixmap(0, 0, buffer_);
}
} // namespace FirstYear::UI
