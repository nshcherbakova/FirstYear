#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Photo &photo) {
  photo_ = photo;
  update();
}

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
  QRectF dirty_rect = geometry().toRectF();
  QRectF image_rect = dirty_rect;
  image_rect.setWidth(dirty_rect.width() * photo_.scale);
  image_rect.setHeight(dirty_rect.height() * photo_.scale);
  image_rect.moveTo(photo_.offset);

  QTransform tr;
  QPointF dp = dirty_rect.center();
  tr.translate(dp.x(), dp.y());
  tr.rotate(photo_.angle, Qt::ZAxis);
  tr.translate(-dp.x(), -dp.y());
  painter.setTransform(tr);

  painter.drawPixmap(dirty_rect, photo_.image, image_rect);
}
} // namespace FirstYear::UI
