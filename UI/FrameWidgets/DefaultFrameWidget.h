#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <QWidget>

namespace FirstYear::UI{
class DefaultFrameWidget final : public QWidget{
  Q_OBJECT
public:

  explicit DefaultFrameWidget(QWidget &parent);
  DefaultFrameWidget &operator=(const DefaultFrameWidget &) = delete;

public: // IWidget
  //virtual void onShow(const bool visible) override final;

signals:


public slots:


private: // QWidget
  virtual void paintEvent(QPaintEvent *event) override final;

private:


private:

};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
