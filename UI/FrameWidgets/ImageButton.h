#pragma once
#ifndef FIRSTYEAR_UI_IMAGE_BUTTON_H
#define FIRSTYEAR_UI_IMAGE_BUTTON_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QPushButton>
#include <UI/FrameWidgets/PhotoTuneWidget.h>

namespace FirstYear::UI {

class ImageButton : public QPushButton, public PhotoPainter {
  Q_OBJECT
public:
  explicit ImageButton(QWidget &parent);
  ImageButton &operator=(const ImageButton &) = delete;

public:
  void setPhoto(const Core::PhotoData &photo);
  void paintEvent(QPaintEvent *);
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_IMAGE_BUTTON_H
