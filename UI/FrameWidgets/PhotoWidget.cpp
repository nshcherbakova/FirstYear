#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background-color: rgba(255, 255, 255, 0);}";
static const char *c_text_widget_style_str =
    "QLabel{ background-color: rgba(255, 255, 255, 0);}";

PhotoWidget::PhotoWidget(QWidget &parent)
    : QWidget(&parent), image_widget_(*this), text_widget_(this) {

  setContentsMargins(0, 0, 0, 0);

  image_widget_.setContentsMargins(0, 0, 0, 0);
  image_widget_.setStyleSheet(c_image_widget_button_style_str);
  connect(&image_widget_, &QPushButton::clicked, this,
          &PhotoWidget::SignalImagePressed);

  text_widget_.setStyleSheet(c_text_widget_style_str);
  text_widget_.setAlignment(Qt::AlignCenter);
}

void PhotoWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  image_widget_.setGeometry({QPoint(), geometry().size()});
  text_widget_.setGeometry({0, 100, 100, 20});
  update();
}

void PhotoWidget::setPhoto(const Core::PhotoData &photo, QRect boundary_rect) {
  photo_ = photo;
  image_widget_.setPhoto(photo, boundary_rect);
  update();
}

void PhotoWidget::setText(QString text) { text_widget_.setText(text); }

} // namespace FirstYear::UI
