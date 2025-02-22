#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background: transparent;"
    "}";

PhotoWidget::PhotoWidget(QWidget &parent, bool render_state)
    : ImageButton(parent), render_state_(render_state) {

  if (!render_state) {
    setAcceptDrops(true);
  }
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(c_image_widget_button_style_str);
  connect(this, &QPushButton::clicked, this, &PhotoWidget::SignalImagePressed);

  timer_.setInterval(200);
  timer_.setSingleShot(true);

  connect(&timer_, &QTimer::timeout, this, [&] {
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << id_;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->setPixmap(photo_scaled_);
    drag->setHotSpot({size().width() / 2, size().height() / 2});

    drag->exec(Qt::CopyAction);
  });

  if (!render_state) {
    svg_render_open_ = new QSvgRenderer(this);
    svg_render_open_->load(QString(":/images/icons/open"));
    svg_render_edit_image_ = new QSvgRenderer(this);
    svg_render_edit_image_->load(QString(":/images/icons/edit_image"));
  }
}

void PhotoWidget::resizeEvent(QResizeEvent *event) {
  ImageButton::resizeEvent(event);

  update();
}

void PhotoWidget::setPhoto(const Core::PhotoDataPtr &photo, int id) {
  photo_data_ = photo;

  id_ = id;

  ImageButton::setPhoto(photo);
}

void PhotoWidget::OnUpdateImageBuffer(QPixmap &buffer) {

  QPainter painter(&buffer);

  if (!render_state_) {
    photo_scaled_ = buffer;

    QSvgRenderer *render =
        photo_data_->isStub() ? svg_render_open_ : svg_render_edit_image_;

    const auto size = parentWidget()->size().width() / 16;
    render->render(&painter,
                   QRect{rect().bottomRight() - QPoint{size, size} * 1.1,
                         QSize{size, size}});
  }
  auto gradient_height = this->parentWidget()->size().height() / 15.0;
  QLinearGradient gradient(0, -10, 0, gradient_height);
  gradient.setColorAt(0.0, QColor(76, 82, 32, 200));
  gradient.setColorAt(1.0, QColor(76, 82, 32, 0));

  painter.fillRect(QRect(0, 0, width(), gradient_height), gradient);
}

void PhotoWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (isVisible() &&
      event->mimeData()->hasFormat("application/x-dnditemdata")) {
    if (event->source() == this) {
      event->ignore();
    } else {
      event->acceptProposedAction();
      drag_enter_ = true;
      update();
    }
  } else {
    event->ignore();
    drag_enter_ = false;
  }
}

void PhotoWidget::dragLeaveEvent(QDragLeaveEvent *) {
  drag_enter_ = false;
  update();
}

void PhotoWidget::dragMoveEvent(QDragMoveEvent *event) {
  if (isVisible() &&
      event->mimeData()->hasFormat("application/x-dnditemdata")) {
    if (event->source() == this) {
      event->ignore();
    } else {
      event->acceptProposedAction();
    }
  } else {
    drag_enter_ = false;
    event->ignore();
  }
}

void PhotoWidget::dropEvent(QDropEvent *event) {
  drag_enter_ = false;

  if (event->source() == this) {
    event->ignore();
    return;
  }
  if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    int id = 0;
    dataStream >> id;

    emit SignalImageDroped(id);

    if (event->source() == this) {
      event->ignore();
    } else {
      event->acceptProposedAction();
    }
  } else {
    event->ignore();
  }
}

//! [1]
void PhotoWidget::mousePressEvent(QMouseEvent *event) {
  if (isVisible() && !render_state_) {
    timer_.start();
  }
  ImageButton::mousePressEvent(event);
}

void PhotoWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (!render_state_) {
    timer_.stop();
  }
  ImageButton::mouseReleaseEvent(event);
}

void PhotoWidget::paintEvent(QPaintEvent *e) {

  ImageButton::paintEvent(e);
  if (!render_state_ && drag_enter_) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(200, 200, 200, 200));

  } else if (!render_state_) {
    QPushButton::paintEvent(e);
  }
}
} // namespace FirstYear::UI
