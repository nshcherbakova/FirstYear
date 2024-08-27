#include "PhotoTuneWidget.h"
#include <QGestureEvent>
#include <stdafx.h>
namespace FirstYear::UI {

PhotoTuneWidget::PhotoTuneWidget(QWidget &parent) : QWidget(&parent) {

  QList<Qt::GestureType> gestures;

  gestures << Qt::PanGesture;
  gestures << Qt::PinchGesture;
  grabGestures(gestures);

  auto palette = QWidget::palette();
  //  palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);
  // setContentsMargins(10, 10, 10, 10);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);

  double zoom_step = 0.1;
  double ZOOM_MIN = 0.1;
  double ZOOM_MAX = 10.0;

  double move_step = 10;

  double rotate_step = 0.1;

  int button_with = 40;
  int button_margin = 20;
  int button_space = 10;
  auto zoom_in = new QPushButton(this);
  zoom_in->setGeometry(button_margin, button_margin, button_with, button_with);
  zoom_in->setText("+");
  zoom_in->setContentsMargins(0, 0, 0, 0);
  connect(zoom_in, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MAX]() {
    if (this->photo_.scale < ZOOM_MAX) {
      this->photo_.scale += zoom_step;
      this->update();
    }
  });

  auto zoom_out = new QPushButton(this);
  zoom_out->setGeometry(button_margin,
                        button_margin + button_with + button_space, button_with,
                        button_with);
  zoom_out->setText("-");
  zoom_out->setContentsMargins(0, 0, 0, 0);
  connect(zoom_out, &QPushButton::clicked, this, [&, zoom_step, ZOOM_MIN]() {
    if (this->photo_.scale >= ZOOM_MIN) {
      this->photo_.scale -= zoom_step;
      this->update();
    }
  });

  auto left = new QPushButton(this);
  left->setGeometry(
      geometry().width() - button_margin - 3 * button_with - 2 * button_space,
      button_space + button_margin + button_with, button_with, button_with);
  left->setText("<");
  left->setContentsMargins(0, 0, 0, 0);
  connect(left, &QPushButton::clicked, this, [&, move_step]() {
    //  if(this->photo_.image.width()*this->photo_.scale +
    //  this->>photo_.offset.x() > FRAME_RIGHT)
    {
      this->photo_.offset -= {move_step, 0.0};
      this->update();
    }
  });

  auto right = new QPushButton(this);
  right->setGeometry(geometry().width() - button_margin - button_with,
                     button_space + button_margin + button_with, button_with,
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
  top->setGeometry(geometry().width() - button_margin - 2 * button_with -
                       button_space,
                   button_margin, button_with, button_with);
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
  bottom->setGeometry(geometry().width() - button_margin - 2 * button_with -
                          button_space,
                      2 * button_space + button_margin + 2 * button_with,
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
  rotate->setGeometry(geometry().width() - button_margin - 2 * button_with -
                          button_space,
                      2 * button_space + button_margin + 4 * button_with,
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

  auto open_file = new QPushButton(this);
  open_file->setGeometry(button_margin, geometry().height() - 2 * button_with,
                         2 * button_with, button_with);
  open_file->setText("Open");
  open_file->setContentsMargins(0, 0, 0, 0);
  connect(open_file, &QPushButton::clicked, this,
          &PhotoTuneWidget::SignalPhotoChanged);

  auto close = new QPushButton(this);
  close->setGeometry(geometry().width() - 3 * button_with,
                     geometry().height() - 2 * button_with, 2 * button_with,
                     button_with);
  close->setText("Close");
  close->setContentsMargins(0, 0, 0, 0);
  connect(close, &QPushButton::clicked, this, [&]() {
    hide();
    emit SignalImageTuned();
  });
}

void PhotoTuneWidget::setPhoto(int id, const Core::PhotoData &photo) {
  id_ = id;
  photo_ = photo;
  QRectF photo_rect = photo.image.rect();
  QRectF widget_rect = rect();
  double k1 = photo_rect.width() / photo_rect.height();
  double k2 = widget_rect.width() / widget_rect.height();

  internal_scale_ = 1;
  internal_offset_ = QPoint();

  if (k1 < k2) {
    internal_scale_ = (double)widget_rect.height() / photo_rect.height();
  } else {
    internal_scale_ = (double)widget_rect.width() / photo_rect.width();
  }

  internal_offset_ = (QPoint(photo_rect.width() * internal_scale_,
                             photo_rect.height() * internal_scale_) -
                      QPoint(widget_rect.width(), widget_rect.height())) /
                     2;
  update();
}
int PhotoTuneWidget::getPhotoId() const { return id_; }
Core::PhotoData PhotoTuneWidget::getPhoto() const { return photo_; }

void PhotoTuneWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (photo_.image.isNull())
    return;

  // QTransform tr;
  //   painter.translate(-internal_offset_.x(), -internal_offset_.y());
  //  painter.scale(internal_scale_, internal_scale_);
  //  Draw background
  /* QRectF dirty_rect = rect().toRectF();
   QRectF image_rect = photo_.image.rect();

   tr.translate(photo_.offset.x(), -photo_.offset.y());

   QPointF dp = dirty_rect.center();
   tr.translate(dp.x(), dp.y());
   tr.rotate(photo_.angle, Qt::ZAxis);
   tr.scale(photo_.scale, photo_.scale);
   tr.translate(-dp.x(), -dp.y());*/

  const qreal iw = photo_.image.width();
  const qreal ih = photo_.image.height();
  const qreal wh = height();
  const qreal ww = width();

  painter.translate(ww / 2, wh / 2);
  painter.translate(photo_.offset.x(), photo_.offset.y());
  painter.rotate(photo_.angle);
  painter.scale(internal_scale_, internal_scale_);
  painter.scale(currentStepScaleFactor * photo_.scale,
                currentStepScaleFactor * photo_.scale);
  painter.translate(-iw / 2, -ih / 2);

  painter.drawPixmap(0, 0, photo_.image);
}

void PhotoTuneWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  hide();
  emit SignalImageTuned();
}

Q_LOGGING_CATEGORY(lcExample, "qt.examples.imagegestures")

void PhotoTuneWidget::grabGestures(const QList<Qt::GestureType> &gestures) {
  //! [enable gestures]
  for (Qt::GestureType gesture : gestures)
    grabGesture(gesture);
  //! [enable gestures]
}

//! [event handler]
bool PhotoTuneWidget::event(QEvent *event) {
  if (event->type() == QEvent::Gesture)
    return gestureEvent(static_cast<QGestureEvent *>(event));
  return QWidget::event(event);
}
//! [event handler]
//!
//! [gesture event handler]
bool PhotoTuneWidget::gestureEvent(QGestureEvent *event) {
  qCDebug(lcExample) << "gestureEvent():" << event;
  if (QGesture *pan = event->gesture(Qt::PanGesture))
    panTriggered(static_cast<QPanGesture *>(pan));
  if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    pinchTriggered(static_cast<QPinchGesture *>(pinch));
  return true;
}
//! [gesture event handler]

void PhotoTuneWidget::panTriggered(QPanGesture *gesture) {
#ifndef QT_NO_CURSOR
  switch (gesture->state()) {
  case Qt::GestureStarted:
  case Qt::GestureUpdated:
    setCursor(Qt::SizeAllCursor);
    break;
  default:
    setCursor(Qt::ArrowCursor);
  }
#endif
  QPointF delta = gesture->delta();
  qCDebug(lcExample) << "panTriggered():" << gesture;
  photo_.offset += delta;

  update();
}

//! [pinch function]
void PhotoTuneWidget::pinchTriggered(QPinchGesture *gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::RotationAngleChanged) {
    qreal rotationDelta =
        gesture->rotationAngle() - gesture->lastRotationAngle();
    photo_.angle += rotationDelta;
    qCDebug(lcExample) << "pinchTriggered(): rotate by" << rotationDelta << "->"
                       << photo_.angle;
  }
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    currentStepScaleFactor = gesture->totalScaleFactor();
    qCDebug(lcExample) << "pinchTriggered(): zoom by" << gesture->scaleFactor()
                       << "->" << currentStepScaleFactor;
  }
  if (gesture->state() == Qt::GestureFinished) {
    photo_.scale *= currentStepScaleFactor;
    currentStepScaleFactor = 1;
  }
  update();
}
} // namespace FirstYear::UI
