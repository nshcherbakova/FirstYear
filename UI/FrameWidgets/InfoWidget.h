#pragma once
#ifndef FIRSTYEAR_INFO_WIDGET_H
#define FIRSTYEAR_INFO_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <QWidget>

class QLabel;

namespace FirstYear::UI {
class TextButton;

class InfoWidget : public QWidget {
  Q_OBJECT
public:
  explicit InfoWidget(QWidget *parent, Core::FrameControl &frame_control);

  InfoWidget &operator=(const InfoWidget &) = delete;
  virtual ~InfoWidget(){};

signals:
  void SignalShareLog();

public:
  virtual void resizeEvent(QResizeEvent *e) override;
  virtual void mouseReleaseEvent(QMouseEvent *e) override;
  virtual void setVisible(bool visible) override;

private:
  // TextButton *close_ = nullptr;
  Core::FrameControl &frame_control_;
  TextButton *share_ = nullptr;
  QLabel *text_ = nullptr;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_INFO_WIDGET_H
