#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include "ImageButton.h"
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
  void setImage(QPixmap image);
  void setText(QString text);
  void setImageParameters(double scale, QPoint offset);
signals:
  void SignalImagePressed();

public slots:

private: // QWidget
         // virtual void paintEvent(QPaintEvent *event) override final;
private:
  void updateData();

private:
  QPixmap image_;
  QString text_;
  ImageButton image_widget_;
  QLabel text_widget_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
