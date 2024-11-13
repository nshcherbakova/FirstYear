#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include "ImageButton.h"
#include <Core/Project/Project.h>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

namespace FirstYear::UI {

class PhotoWidget final : public ImageButton {
  Q_OBJECT
public:
  explicit PhotoWidget(QWidget &parent, bool render_state);
  PhotoWidget &operator=(const PhotoWidget &) = delete;

public:
  void setText(QString text);
  void setPhoto(const Core::PhotoData &photo);
  Core::PhotoData getPhoto();

public:
  virtual void resizeEvent(QResizeEvent *event) override final;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
signals:
  void SignalImagePressed();

private:
  QLabel text_widget_;
  bool render_state_ = false;
  Core::PhotoData photo_data_;
  QPixmap photo_scaled_;
  QTimer timer_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
