#include "PhotoTuneWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

PhotoTuneWidget::PhotoTuneWidget(QWidget &parent) : QWidget(&parent) {

  setContentsMargins(0, 0, 0, 0);
}

void PhotoTuneWidget::setPhoto(const Photo &photo) { photo_ = photo; }

Photo PhotoTuneWidget::getPhoto() const { return photo_; }

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
  QRectF dirty_rect = geometry().toRectF();
  QRectF image_rect = dirty_rect;
  image_rect.setWidth(dirty_rect.width() * photo_.scale);
  image_rect.setHeight(dirty_rect.height() * photo_.scale);
  image_rect.moveTo(photo_.offset);

  painter.drawPixmap(dirty_rect, photo_.image, image_rect);
}

} // namespace FirstYear::UI
