#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
#include <Core/Project/Project.h>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

namespace FirstYear::UI {

class PhotoTuneWidget final : public QWidget {
  Q_OBJECT
public:
  explicit PhotoTuneWidget(QWidget &parent);
  PhotoTuneWidget &operator=(const PhotoTuneWidget &) = delete;

signals:
  void SignalImageTuned();

public:
  void setPhoto(int id, const Core::PhotoData &photo);
  Core::PhotoData getPhoto() const;
  int getPhotoId() const;

  void paintEvent(QPaintEvent *);
  void mouseDoubleClickEvent(QMouseEvent *event);

private:
  int id_ = 0;
  Core::PhotoData photo_;
  double internal_scale_ = 0;
  QPoint internal_offset_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_TUNE_WIDGET_H
