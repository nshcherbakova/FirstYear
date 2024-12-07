#include "Buttons.h"
#include <stdafx.h>

namespace FirstYear::UI {

TouchebleButton::TouchebleButton(QWidget *parent, bool accept_touch)
    : TouchButton(parent), accept_touch_(accept_touch) {
  setContentsMargins(0, 0, 0, 0);
}

bool TouchebleButton::event(QEvent *event) {
  if (accept_touch_) {
    return TouchButton::event(event);
  } else {
    return QPushButton::event(event);
  }
}

ShareButton::ShareButton(QWidget *parent, bool accept_touch)
    : TouchebleButton(parent, accept_touch) {

  QSize size(90, 90);

  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(
      QString(c_share_button_style_template_str).arg(size.width() / 2));
  setIcon(QIcon(c_share_image_str));

  setMinimumSize(size);
  setMaximumSize(size);
  setGeometry({{}, size});
  setIconSize({55, 55});

  adjustSize();
}

/*
TextButton::TextButton(QWidget *parent, bool accept_touch) :
TouchebleButton(parent, accept_touch) { QSize size(80, 40);

    setContentsMargins(0, 0, 0, 0);

    setMinimumSize(size);
    setMaximumSize(size);
    setGeometry({{},size});
    QFont font ( this->font());
    font.setPointSize(height() / 1.9);
    setFont(font);
}*/

TextButton::TextButton(QWidget *parent, bool accept_touch)
    : TouchebleButton(parent, accept_touch) {

  setContentsMargins(0, 0, 0, 0);

  setSize(QSize(200, 60));

  QFont font(this->font());
  font.setPointSize(width() / 4.0);
  setFont(font);
}

void TextButton::setSize(QSize size) {
  setMinimumSize(size);
  setMaximumSize(size);
  setGeometry({{}, size});
}

void TextButton::resizeEvent(QResizeEvent *e) {
  TouchebleButton::resizeEvent(e);

  QFont font(this->font());
  font.setPointSize(width() / 1.9);
  setFont(font);
}
/*
PreviewButton::PreviewButton(QWidget *parent, bool accept_touch) :
TouchebleButton(parent, accept_touch) {

    QSize size(80, 40);

    setContentsMargins(0, 0, 0, 0);

    setMinimumSize(size);
    setMaximumSize(size);
    setGeometry({{},size});

    setStyleSheet(c_select_button_style_str);

    QFont font ( this->font());
    font.setPointSize(height() / 1.9);
    setFont(font);
}

void SelectButton::resizeEvent(QResizeEvent *)
{
    QFont font ( this->font());
    font.setPointSize(height() / 1.9);
    setFont(font);
}
*/
} // namespace FirstYear::UI
