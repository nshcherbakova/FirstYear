#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background-color: rgba(255, 255, 255, 0);}";

PhotoWidget::PhotoWidget(QWidget &parent)
    : QWidget(&parent), image_widget_(this), text_widget_(this) {

  setContentsMargins(0, 0, 0, 0);
  setMinimumSize({100, 120});
  image_widget_.setGeometry({0, 0, 100, 100});
  image_widget_.setContentsMargins(0, 0, 0, 0);
  image_widget_.setStyleSheet(c_image_widget_button_style_str);
  connect(&image_widget_, &QPushButton::clicked, this,
          &PhotoWidget::SignalImagePressed);

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
    image_ = image_.scaledToHeight(100);
    image_widget_.setIcon(QIcon(image_));
    image_widget_.setIconSize(image_.rect().size());
  }

  if (!text_.isNull()) {
    text_widget_.setText(text_);
  }

  // Draw background
}

} // namespace FirstYear::UI
