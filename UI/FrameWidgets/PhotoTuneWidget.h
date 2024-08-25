#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <UI/FrameWidgets/Photo.h>

namespace FirstYear::UI {

class PhotoTuneWidget final : public QWidget {
  Q_OBJECT
public:
  explicit PhotoTuneWidget(QWidget &parent);
  PhotoTuneWidget &operator=(const PhotoTuneWidget &) = delete;

signals:
  void SignalImageTuned();

public:
  void setPhoto(int id, const Photo &photo);
  Photo getPhoto() const;
  int getPhotoId() const;

  void paintEvent(QPaintEvent *);
  void mouseDoubleClickEvent(QMouseEvent *event);

private:
  int id_ = 0;
  Photo photo_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
