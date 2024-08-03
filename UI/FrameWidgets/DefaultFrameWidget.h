#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <QWidget>

class QGridLayout;

namespace FirstYear::UI {
class PhotoWidget;
class DefaultFrameWidget final : public QWidget {
  Q_OBJECT
public:
  explicit DefaultFrameWidget(QWidget &parent);
  DefaultFrameWidget &operator=(const DefaultFrameWidget &) = delete;

public: // IWidget
        // virtual void onShow(const bool visible) override final;
signals:

public slots:

private:
  void InitPhotos();
  QString OpenFile();

private:
  QGridLayout *layout_;
  std::vector<PhotoWidget *> photos_;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
