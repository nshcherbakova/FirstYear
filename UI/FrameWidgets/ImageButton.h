#pragma once
#ifndef FIRSTYEAR_UI_IMAGE_BUTTON_H
#define FIRSTYEAR_UI_IMAGE_BUTTON_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QPushButton>

namespace FirstYear::UI {

class ImageButton final : public QPushButton {
  Q_OBJECT
public:
  explicit ImageButton(QWidget &parent);
  ImageButton &operator=(const ImageButton &) = delete;

public:
  void setPhoto(const Core::PhotoData &photo);
  void paintEvent(QPaintEvent *);

private:
  Core::PhotoData photo_;
  double internal_scale_ = 0;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_IMAGE_BUTTON_H
