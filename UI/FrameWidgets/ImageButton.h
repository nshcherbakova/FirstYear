#pragma once
#ifndef FIRSTYEAR_UI_IMAGE_BUTTON_H
#define FIRSTYEAR_UI_IMAGE_BUTTON_H
#include <QPixmap>
#include <QPushButton>

namespace FirstYear::UI {
class ImageButton final : public QPushButton {
  Q_OBJECT
public:
  explicit ImageButton(QWidget &parent);
  ImageButton &operator=(const ImageButton &) = delete;

public:
  void setImage(QPixmap image);
  void setImageParameters(double scale, QPoint offset);
  void paintEvent(QPaintEvent *);

private:
  QPixmap image_;
  double scale_ = 1;
  QPoint offset_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_IMAGE_BUTTON_H
