#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_image_widget_button_style_str =
    "QPushButton{ background-color: rgba(255, 255, 255, 0);"
    "}";

static const char *c_text_widget_style_str =
    "QLabel{ background-color: rgba(255, 255, 255, 0);}";

PhotoWidget::PhotoWidget(QWidget &parent, bool render_state)
    : ImageButton(parent), text_widget_(this), render_state_(render_state) {

  if (!render_state) {
    setAcceptDrops(true);
  }
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(c_image_widget_button_style_str);
  connect(this, &QPushButton::clicked, this, &PhotoWidget::SignalImagePressed);

  text_widget_.setStyleSheet(c_text_widget_style_str);
  text_widget_.setAlignment(Qt::AlignCenter);
  text_widget_.setVisible(!render_state_);

  timer_.setInterval(200);
  timer_.setSingleShot(true);

  connect(&timer_, &QTimer::timeout, this, [&] {
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << id_; // << QPoint(event->position().toPoint() - pos());
    //! [1]

    //! [2]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    //! [2]

    //! [3]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->setPixmap(photo_scaled_);
    drag->setHotSpot({size().width() / 2, size().height() / 2});
    //! [3]

    drag->exec(Qt::CopyAction);
  });
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

void PhotoWidget::setPhoto(const Core::PhotoDataPtr &photo, int id) {
  photo_data_ = photo;

  id_ = id;

  ImageButton::setPhoto(photo);
  if (!render_state_) {
    photo_scaled_ = ImageButton::grab();
  }

  if (photo->isStub()) {
    setText("Open Image");
  } else {
    setText("Edit");
  }
}

void PhotoWidget::setText(QString text) { text_widget_.setText(text); }

void PhotoWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    if (event->source() == this) {
      //  event->setDropAction(Qt::MoveAction);
      //  event->accept();
      event->ignore();
    } else {
      event->acceptProposedAction();
      drag_enter_ = true;
      update();
    }
  } else {
    event->ignore();
  }
}

void PhotoWidget::dragLeaveEvent(QDragLeaveEvent *) {
  drag_enter_ = false;
  update();
}

void PhotoWidget::dragMoveEvent(QDragMoveEvent *event) {
  if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    if (event->source() == this) {
      //   event->setDropAction(Qt::MoveAction);
      event->ignore();
    } else {
      event->acceptProposedAction();
    }
  } else {
    event->ignore();
  }
}

void PhotoWidget::dropEvent(QDropEvent *event) {
  drag_enter_ = false;
  // update();
  if (event->source() == this) {
    // event->setDropAction(Qt::MoveAction);
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
      // event->setDropAction(Qt::MoveAction);
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
  if (!render_state_) {
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
  }
}
} // namespace FirstYear::UI
