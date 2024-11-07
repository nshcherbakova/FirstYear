#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background-color: rgba(255, 255, 255, 0);}";
static const char *c_text_widget_style_str =
    "QLabel{ background-color: rgba(255, 255, 255, 0);}";

PhotoWidget::PhotoWidget(QWidget &parent, bool render_state)
    : ImageButton(parent), text_widget_(this), render_state_(render_state) {

  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(c_image_widget_button_style_str);
  connect(this, &QPushButton::clicked, this, &PhotoWidget::SignalImagePressed);

  text_widget_.setStyleSheet(c_text_widget_style_str);
  text_widget_.setAlignment(Qt::AlignCenter);
  text_widget_.setVisible(!render_state_);
}

void PhotoWidget::resizeEvent(QResizeEvent *event) {
  ImageButton::resizeEvent(event);

  QFont font = text_widget_.font();
  font.setPointSizeF(size().height() / 7.0);
  text_widget_.setFont(font);
  text_widget_.adjustSize();

  text_widget_.setGeometry(
      {0, (int)(size().height() / 1.5) - text_widget_.height(), size().width(),
       text_widget_.height()});

  update();
}

void PhotoWidget::setPhoto(const Core::PhotoData &photo) {
  ImageButton::setPhoto(photo);
  if (photo.is_stub_image) {
    setText("Open Image");
  } else {
    setText("Edit");
  }
}

void PhotoWidget::setText(QString text) { text_widget_.setText(text); }

} // namespace FirstYear::UI
