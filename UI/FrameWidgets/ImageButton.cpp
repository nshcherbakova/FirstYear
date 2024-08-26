#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Core::PhotoData &photo) {
  photo_ = photo;

  QRectF photo_rect = photo.image.rect();
  QRectF widget_rect = rect();
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = widget_rect.width() / widget_rect.height();

  internal_scale_ = 1;
  internal_offset_ = QPoint();

  if (k1 < k2) {
    internal_scale_ = (double)widget_rect.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)widget_rect.width() / photo_rect.width();
  }

  internal_offset_ = (QPoint(photo_rect.width() * internal_scale_,
                             photo_rect.height() * internal_scale_) -
                      QPoint(widget_rect.width(), widget_rect.height())) /
                     2;

  update();
}

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  QTransform tr;
  tr.translate(-internal_offset_.x(), -internal_offset_.y());
  tr.scale(internal_scale_, internal_scale_);

  tr.translate(photo_.offset.x(), -photo_.offset.y());

  QPointF dp = rect().toRectF().center();
  tr.translate(dp.x(), dp.y());
  tr.rotate(photo_.angle, Qt::ZAxis);
  tr.scale(photo_.scale, photo_.scale);
  tr.translate(-dp.x(), -dp.y());
  painter.setTransform(tr);

  painter.drawPixmap(0, 0, photo_.image);
}
} // namespace FirstYear::UI
