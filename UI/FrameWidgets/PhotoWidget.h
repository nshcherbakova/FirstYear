#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include "ImageButton.h"
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <UI/FrameWidgets/Photo.h>

namespace FirstYear::UI {

class PhotoWidget final : public QWidget {
  Q_OBJECT
public:
  explicit PhotoWidget(QWidget &parent);
  PhotoWidget &operator=(const PhotoWidget &) = delete;

public:
  void setText(QString text);
  void setPhoto(const Photo &photo);
  Photo getPhoto();
signals:
  void SignalImagePressed();

private:
  Photo photo_;
  ImageButton image_widget_;
  QLabel text_widget_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
