#pragma once
#ifndef FIRSTYEAR_UI_IMAGE_BUTTON_H
#define FIRSTYEAR_UI_IMAGE_BUTTON_H
#include <QPixmap>
#include <QPushButton>
#include <UI/FrameWidgets/Photo.h>

namespace FirstYear::UI {

class ImageButton final : public QPushButton {
  Q_OBJECT
public:
  explicit ImageButton(QWidget &parent);
  ImageButton &operator=(const ImageButton &) = delete;

public:
  void setPhoto(const Photo &photo);
  void paintEvent(QPaintEvent *);

private:
  Photo photo_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_IMAGE_BUTTON_H
