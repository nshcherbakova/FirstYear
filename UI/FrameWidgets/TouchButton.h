#pragma once
#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#include <QPushButton>

namespace FirstYear::UI {

class TouchButton : public QPushButton {
public:
  TouchButton(QWidget *parent);
  bool event(QEvent *event);
};

} // namespace FirstYear::UI
#endif // TOUCH_BUTTON_H
