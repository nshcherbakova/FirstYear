#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setImage(QPixmap image) {
  image_ = image;
  update();
}

void ImageButton::setImageParameters(double scale, QPoint offset) {
  scale_ = scale;
  offset_ = offset;
  update();
}

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
  QRectF dirty_rect = geometry().toRectF();
  QRectF image_rect = dirty_rect;
  image_rect.setWidth(dirty_rect.width() * scale_);
  image_rect.setHeight(dirty_rect.height() * scale_);
  image_rect.moveTo(offset_);

  painter.drawPixmap(dirty_rect, image_, image_rect);
}
} // namespace FirstYear::UI
