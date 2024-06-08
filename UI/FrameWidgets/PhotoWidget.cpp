#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

PhotoWidget::PhotoWidget(QWidget &parent)
    : QWidget(&parent), image_(":images/frame/month_stub") {

  setContentsMargins(0, 0, 0, 0);
  auto palette = QWidget::palette();
  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);

  setMinimumSize({100, 100});
  update();
}
/*
void PhotoWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
}*/

void PhotoWidget::setImage(QImage image) {
  // image_ = image;
  update();
}

void PhotoWidget::setText(QString text) {
  text_ = text;
  update();
}

void PhotoWidget::update() {
  if (!image_.isNull()) {
    // setImage(image_);

    auto label = new QLabel(this);
    label->setPixmap(image_);
  }
  if (!text_.isNull()) {
    auto label = new QLabel(this);
    label->setText(text_);
  }

  // Draw background
}
} // namespace FirstYear::UI
