#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include "ImageButton.h"
#include <Core/Project/Project.h>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

namespace FirstYear::UI {

class PhotoWidget final : public QWidget {
  Q_OBJECT
public:
  explicit PhotoWidget(QWidget &parent);
  PhotoWidget &operator=(const PhotoWidget &) = delete;

public:
  void setText(QString text);
  void setPhoto(const Core::PhotoData &photo, QRect boundary_rect);
  Core::PhotoData getPhoto();

public:
  virtual void resizeEvent(QResizeEvent *event) override final;
signals:
  void SignalImagePressed();

private:
  Core::PhotoData photo_;
  ImageButton image_widget_;
  QLabel text_widget_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
