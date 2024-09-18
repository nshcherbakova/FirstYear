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

public: // IWidget
        // virtual void onShow(const bool visible) override final;
signals:

public slots:

private:
  void InitPhotos(Core::FrameControl &control);
  QString OpenFile();
  QPixmap GetStubPhoto(int month);

private:
  QGridLayout *layout_ = nullptr;
  std::vector<PhotoWidget *> photos_;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  ClickableLabel *myLabel_ = nullptr;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
