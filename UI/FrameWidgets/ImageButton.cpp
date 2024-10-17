#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Core::PhotoData &photo) {
  photo_data_ = photo;
  PhotoPainter::init(photo, {{}, rect().size()}, {{}, rect().size()});

  UpdateButton();
}

void ImageButton::UpdateButton() {
  if (buffer_.size() != rect().size()) {
    buffer_ = QPixmap(rect().size() * devicePixelRatio());
    buffer_.setDevicePixelRatio(devicePixelRatio());
  }

  QPainter painter(&buffer_);
  const auto rect = QRect(0, 0, buffer_.width(), buffer_.height());
  spdlog::info("update rect {} {}", rect.width(), rect.height());
  painter.setBrush(Qt::white);
  painter.drawPolygon(rect);
  PhotoPainter::drawPhoto(painter);
  update();
}

void ImageButton::resizeEvent(QResizeEvent *) { UpdateButton(); }

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  const auto rect = QRect(0, 0, buffer_.width(), buffer_.height());
  spdlog::info("paint rect {} {}", rect.width(), rect.height());
  spdlog::info("paint rect {} {}", rect.width(), rect.height());
  painter.drawPixmap(0, 0, buffer_);
}
} // namespace FirstYear::UI
