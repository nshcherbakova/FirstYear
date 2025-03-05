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
class QSvgRenderer;
QT_END_NAMESPACE

namespace FirstYear::UI {

class PhotoWidget final : public ImageButton {
  Q_OBJECT
public:
  struct Parameters {
    bool accept_drops = false;
    bool show_mons_number = false;
    bool show_edit_icon = false;
    bool show_open_icon = false;
    bool show_shadow = false;
  };

  explicit PhotoWidget(QWidget &parent, Parameters parameters);
  PhotoWidget &operator=(const PhotoWidget &) = delete;

public:
  void setPhoto(const Core::PhotoDataPtr &photo, int id);
  Core::PhotoData getPhoto();

public:
  virtual void resizeEvent(QResizeEvent *event) override final;
  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
  virtual void dragMoveEvent(QDragMoveEvent *event) override;
  virtual void dropEvent(QDropEvent *event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void paintEvent(QPaintEvent *) override;

signals:
  void SignalImagePressed();
  void SignalImageDroped(int);

protected:
  virtual void OnUpdateImageBuffer(QPixmap &) override;

private:
  Parameters parameters_;
  Core::PhotoDataPtr photo_data_;
  QPixmap photo_scaled_;
  QTimer timer_;
  int id_ = 0;
  bool drag_enter_ = false;
  QSvgRenderer *svg_render_edit_image_ = nullptr;
  QSvgRenderer *svg_render_open_ = nullptr;
  QPixmap icon_buffer_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
