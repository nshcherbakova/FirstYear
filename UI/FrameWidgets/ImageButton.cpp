#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Core::PhotoDataPtr &photo) {
  photo_data_ = photo;
  PhotoPainter::init(photo, rect(), rect());

  UpdateButton();
}

void ImageButton::UpdateButton() {
  if (buffer_.size() != rect().size()) {
    buffer_ = QPixmap(rect().size() * devicePixelRatio());
    buffer_.setDevicePixelRatio(devicePixelRatio());
  }

  QPainter painter(&buffer_);
  const auto rect = QRect(0, 0, buffer_.width(), buffer_.height());

  painter.setBrush(QColor(c_photo_widget_background_color));
  painter.drawPolygon(rect);
  PhotoPainter::drawPhoto(painter);
  update();
}

void ImageButton::resizeEvent(QResizeEvent *e) {
  QPushButton::resizeEvent(e);
  UpdateButton();
}

void ImageButton::paintEvent(QPaintEvent *) {

  QPainter painter(this);
  // painter.begin();
  painter.drawPixmap(0, 0, buffer_);
  //  painter.end();
}
} // namespace FirstYear::UI
