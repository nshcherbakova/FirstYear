#pragma once
#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#include <QPushButton>

namespace FirstYear::UI {

class TouchButton : public QPushButton {
public:
  TouchButton(QWidget *parent);
  virtual bool event(QEvent *event) override;
};

} // namespace FirstYear::UI
#endif // TOUCH_BUTTON_H
