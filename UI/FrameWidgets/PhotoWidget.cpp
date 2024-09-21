#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background-color: rgba(255, 255, 255, 0);}";
static const char *c_text_widget_style_str =
    "QLabel{ background-color: rgba(255, 255, 255, 0);}";

PhotoWidget::PhotoWidget(QWidget &parent)
    : ImageButton(parent), text_widget_(this) {

  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(c_image_widget_button_style_str);
  connect(this, &QPushButton::clicked, this, &PhotoWidget::SignalImagePressed);

  text_widget_.setStyleSheet(c_text_widget_style_str);
  text_widget_.setAlignment(Qt::AlignCenter);
}

void PhotoWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  text_widget_.setGeometry({0, 100, 100, 20});

  update();
}

void PhotoWidget::setPhoto(const Core::PhotoData &photo) {
  ImageButton::setPhoto(photo);
}

void PhotoWidget::setText(QString text) { text_widget_.setText(text); }

} // namespace FirstYear::UI
