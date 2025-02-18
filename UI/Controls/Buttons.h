#pragma once
#ifndef FE_SHARE_BUTTON_H
#define FE_SHARE_BUTTON_H

#include <UI/FrameWidgets/TouchButton.h>

class QSvgWidget;
namespace FirstYear::UI {

class TouchebleButton : public TouchButton {
public:
  TouchebleButton(QWidget *parent, bool accept_touch = false);
  virtual bool event(QEvent *event) override;

protected:
  bool accept_touch_ = false;
};

class ShareButton : public TouchebleButton {
public:
  ShareButton(QWidget *parent, bool accept_touch = false);

private:
  QSvgWidget *background_svg_ = nullptr;
};

class TextButton : public TouchebleButton {
public:
  TextButton(QWidget *parent, bool accept_touch = false);

public:
  void setSize(QSize size);

public:
  virtual void resizeEvent(QResizeEvent *event) override;
};
} // namespace FirstYear::UI
#endif // FE_SHARE_BUTTON_H
