#include "DefaultFrameWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

DefaultFrameWidget::DefaultFrameWidget(QWidget &parent)
    : QWidget(&parent), layout_(new QGridLayout()) {

  setContentsMargins(0, 0, 0, 0);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  // palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);

  InitPhotos();

  spdlog::info("DefaultFrameWidget UI created");
}

void DefaultFrameWidget::InitPhotos() {

  layout_->setSizeConstraint(QLayout::SetFixedSize);
  photos_.resize(12);

  int row = 0;
  int column = 0;
  for (size_t i = 0; i < photos_.size(); i++) {
    auto &photo = photos_[i];
    photo = new PhotoWidget(*this);
    photo->setText(QString("%1").arg(i));
    photo->show();

    layout_->addWidget(photo, row, column);
    if (column == 3) {
      column = 0;
      row++;
    } else {
      column++;
    }
  }

  //  layout_->setGeometry(geometry());
  //   layout_->addWidget(new QLabel(":images/frame/month_stub"));
  setLayout(layout_);
}

/*
void DefaultFrameWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
}*/

} // namespace FirstYear::UI
