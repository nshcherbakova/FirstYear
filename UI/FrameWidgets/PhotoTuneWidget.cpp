#include "PhotoTuneWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

PhotoTuneWidget::PhotoTuneWidget(QWidget &parent) : QWidget(&parent) {

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  // setContentsMargins(10, 10, 10, 10);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  double zoom_step = 0.1;
  double ZOOM_MIN = 0.1;
  double ZOOM_MAX = 3.0;

  int move_step = 10;

  double rotate_step = 0.1;

  int button_with = 40;
  int buttom_margin = 20;
  int button_space = 10;
  auto zoom_in = new QPushButton(this);
  zoom_in->setGeometry(buttom_margin, buttom_margin, button_with, button_with);
  zoom_in->setText("+");
  zoom_in->setContentsMargins(0, 0, 0, 0);
  connect(zoom_in, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MIN]() {
    if (this->photo_.scale >= ZOOM_MIN) {
      this->photo_.scale -= zoom_step;
      this->update();
    }
  });

  auto zoom_out = new QPushButton(this);
  zoom_out->setGeometry(buttom_margin,
                        buttom_margin + button_with + button_space, button_with,
                        button_with);
  zoom_out->setText("-");
  zoom_out->setContentsMargins(0, 0, 0, 0);
  connect(zoom_out, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MAX]() {
    if (this->photo_.scale < ZOOM_MAX) {
      this->photo_.scale += zoom_step;
      this->update();
    }
  });

  auto left = new QPushButton(this);
  left->setGeometry(
      geometry().width() - buttom_margin - 3 * button_with - 2 * button_space,
      button_space + buttom_margin + button_with, button_with, button_with);
  left->setText("<");
  left->setContentsMargins(0, 0, 0, 0);
  connect(left, &QPushButton::clicked, this, [&, move_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.offset -= {move_step, 0};
      this->update();
    }
  });

  auto right = new QPushButton(this);
  right->setGeometry(geometry().width() - buttom_margin - button_with,
                     button_space + buttom_margin + button_with, button_with,
                     button_with);
  right->setText(">");
  right->setContentsMargins(0, 0, 0, 0);
  connect(right, &QPushButton::clicked, this, [&, move_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.offset += {move_step, 0};
      this->update();
    }
  });

  auto top = new QPushButton(this);
  top->setGeometry(geometry().width() - buttom_margin - 2 * button_with -
                       button_space,
                   buttom_margin, button_with, button_with);
  top->setText("^");
  top->setContentsMargins(0, 0, 0, 0);
  connect(top, &QPushButton::clicked, this, [&, move_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.offset += {0, move_step};
      this->update();
    }
  });

  auto bottom = new QPushButton(this);
  bottom->setGeometry(geometry().width() - buttom_margin - 2 * button_with -
                          button_space,
                      2 * button_space + buttom_margin + 2 * button_with,
                      button_with, button_with);
  bottom->setText("v");
  bottom->setContentsMargins(0, 0, 0, 0);

  connect(bottom, &QPushButton::clicked, this, [&, move_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.offset -= {0, move_step};
      this->update();
    }
  });

  auto rotate = new QPushButton(this);
  rotate->setGeometry(geometry().width() - buttom_margin - 2 * button_with -
                          button_space,
                      2 * button_space + buttom_margin + 4 * button_with,
                      button_with, button_with);
  rotate->setText("r");
  rotate->setContentsMargins(0, 0, 0, 0);
  connect(rotate, &QPushButton::clicked, this, [&, rotate_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.angle += rotate_step;
      this->update();
    }
  });
}

void PhotoTuneWidget::setPhoto(const Photo &photo) {
  photo_ = photo;
  QRectF photo_rect = photo.image.rect();
  QRectF widget_rect = rect();
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = widget_rect.width() / widget_rect.height();

  photo_.offset = QPoint();
  if (k1 > k2) {
    photo_.scale = photo_rect.height() / widget_rect.height();
    photo_.offset.setX(photo_rect.width() - widget_rect.width() * photo_.scale);
  } else {
    photo_.scale = photo_rect.width() / widget_rect.width();
    photo_.offset.setY(photo_rect.height() -
                       widget_rect.height() * photo_.scale);
  }
}

Photo PhotoTuneWidget::getPhoto() const { return photo_; }

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (photo_.image.isNull())
    return;

  // Draw background
  QRectF dirty_rect = rect().toRectF();
  QRectF image_rect = photo_.image.rect();
  image_rect.setWidth(dirty_rect.width() * photo_.scale);
  image_rect.setHeight(dirty_rect.height() * photo_.scale);
  image_rect.moveTo(photo_.offset);

  QTransform tr;

  QPointF dp = dirty_rect.center();

  tr.translate(dp.x(), dp.y());
  tr.rotate(photo_.angle, Qt::ZAxis);
  tr.translate(-dp.x(), -dp.y());

  painter.setTransform(tr);

  painter.drawPixmap(dirty_rect, photo_.image, image_rect);
}

void PhotoTuneWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  setVisible(false);
}
} // namespace FirstYear::UI
