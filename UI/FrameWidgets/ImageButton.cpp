#include "ImageButton.h"
#include <stdafx.h>

namespace FirstYear::UI {

ImageButton::ImageButton(QWidget &parent) : QPushButton(&parent) {}

void ImageButton::setPhoto(const Core::PhotoData &photo) {
  photo_data_ = photo;
  PhotoPainter::init(photo, {{}, rect().size()}, {{}, rect().size()});
  update();
}

void ImageButton::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  PhotoPainter::drawPhoto(painter);
}
} // namespace FirstYear::UI
