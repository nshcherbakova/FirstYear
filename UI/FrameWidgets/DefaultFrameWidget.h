#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <QWidget>
#include <Types.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>

namespace FirstYear::UI {

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
  QGridLayout *layout_;
  std::vector<PhotoWidget *> photos_;
  PhotoTuneWidget *photo_tune_widget_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
