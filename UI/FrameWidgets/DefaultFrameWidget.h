#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <QLabel>
#include <QWidget>
#include <Types.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>

namespace FirstYear::UI {
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget *parent = Q_NULLPTR,
                          Qt::WindowFlags f = Qt::WindowFlags());
  ~ClickableLabel();

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent *event);
};
class DefaultFrameWidget final : public QWidget {
  Q_OBJECT
public:
  explicit DefaultFrameWidget(QWidget &parent, Core::FrameControl &control);
  DefaultFrameWidget &operator=(const DefaultFrameWidget &) = delete;

public: // QWidget
        // virtual void onShow(const bool visible) override final;
  virtual void paintEvent(QPaintEvent *e) override final;
signals:

public slots:

private:
  void InitPhotos(Core::FrameControl &control);
  QString OpenFile();
  QPixmap GetStubPhoto(int month);
  QPixmap renderFrame(FirstYear::Core::ProjectPtr);

private:
  QString id_;
  QPixmap foreground_;
  QPixmap foreground_to_render_;

  std::vector<PhotoWidget *> photo_widgets_;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  ClickableLabel *myLabel_ = nullptr;
  QWidget *foreground_widget_ = nullptr;
  std::vector<QRectF> photo_slots_;
  std::vector<QRectF> photo_slots_real_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
