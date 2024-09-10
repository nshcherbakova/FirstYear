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
  // internal_offset_ = QPoint();

  if (k1 < k2) {
    internal_scale_ = (double)widget_rect.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)widget_rect.width() / photo_rect.width();
  }

  /*  internal_offset_ = (QPoint(photo_rect.width() * internal_scale_,
                               photo_rect.height() * internal_scale_) -
                        QPoint(widget_rect.width(), widget_rect.height())) /
                       2;*/

  update();
}

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = height();
  const qreal ww = width();

  painter.translate(ww / 2, wh / 2);
  painter.translate(photo_.offset.x() * internal_scale_,
                    photo_.offset.y() * internal_scale_);
  painter.rotate(photo_.angle);
  painter.scale(internal_scale_, internal_scale_);
  painter.scale(photo_.scale, photo_.scale);
  painter.translate(-iw / 2, -ih / 2);

  painter.drawPixmap(0, 0, photo_.image);
}
} // namespace FirstYear::UI
