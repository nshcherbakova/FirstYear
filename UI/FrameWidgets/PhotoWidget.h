#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_WIDGET_H
#define FIRSTYEAR_UI_PHOTO_WIDGET_H
#include <QPixmap>
#include <QWidget>

namespace FirstYear::UI {
class PhotoWidget final : public QWidget {
  Q_OBJECT
public:
  explicit PhotoWidget(QWidget &parent);
  PhotoWidget &operator=(const PhotoWidget &) = delete;

public:
  void setImage(QImage image);
  void setText(QString text);

signals:

public slots:

private: // QWidget
         // virtual void paintEvent(QPaintEvent *event) override final;
private:
  void update();

private:
  QPixmap image_;
  QString text_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_WIDGET_H
