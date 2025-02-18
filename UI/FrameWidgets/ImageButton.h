#pragma once
#ifndef FIRSTYEAR_UI_IMAGE_BUTTON_H
#define FIRSTYEAR_UI_IMAGE_BUTTON_H
#include <Core/Project/Project.h>
#include <QPushButton>
#include <UI/FrameWidgets/Photo/PhotoProcessor.h>

namespace FirstYear::UI {

class ImageButton : public QPushButton, public PhotoPainter {
  Q_OBJECT
public:
  explicit ImageButton(QWidget &parent);
  ImageButton &operator=(const ImageButton &) = delete;

public:
  void setPhoto(const Core::PhotoDataPtr &photo);

public:
  virtual void paintEvent(QPaintEvent *) override;
  virtual void resizeEvent(QResizeEvent *) override;

protected:
  virtual void UpdateButton();
  virtual void OnUpdateImageBuffer(QPixmap &buffer) = 0;

private:
  Core::PhotoDataPtr photo_data_;
  QPixmap buffer_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_IMAGE_BUTTON_H
