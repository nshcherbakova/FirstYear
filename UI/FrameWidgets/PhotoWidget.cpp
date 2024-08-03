#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

PhotoWidget::PhotoWidget(QWidget &parent)
    : QWidget(&parent), image_widget_(this), text_widget_(this) {

  setContentsMargins(0, 0, 0, 0);
  setMinimumSize({100, 120});
  image_widget_.setGeometry({0, 0, 100, 100});
  text_widget_.setGeometry({0, 100, 100, 20});
  text_widget_.setAlignment(Qt::AlignCenter);
}

void PhotoWidget::setImage(QPixmap image) {
  image_ = image;
  update();
}

void PhotoWidget::setText(QString text) {
  text_ = text;
  update();
}

void PhotoWidget::update() {
  if (!image_.isNull()) {
    image_ = image_.scaledToWidth(100);
    image_widget_.setPixmap(image_);
  }

  if (!text_.isNull()) {
    text_widget_.setText(text_);
  }

  // Draw background
}
} // namespace FirstYear::UI
