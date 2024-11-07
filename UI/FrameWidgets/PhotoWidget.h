#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include "ImageButton.h"
#include <Core/Project/Project.h>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

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

signals:
  void SignalImagePressed();

private:
  QLabel text_widget_;
  bool render_state_ = false;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
