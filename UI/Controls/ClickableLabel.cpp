#include "ClickableLabel.h"
#include <stdafx.h>

namespace FirstYear::UI {

ClickableLabel::ClickableLabel(QWidget *parent, int font_size,
                               QString font_color, QString font_family,
                               bool hide_edit_icon)
    : QLabel(parent), hide_edit_icon_(hide_edit_icon) {

  font_ = QFont(font_family, font_size, QFont::Normal);
  QLabel::setFont(font_);

  styled_text_ = "<a style=text-decoration:none style=color:%1>%2   </a>";
  styled_text_ = styled_text_.arg(font_color);
  icon_text_ =
      "  <html><img src=':images/icons/edit' width='%1' height='%1'></html>";
}

void ClickableLabel::setText(QString text) {

  text_ = text;
  if (hide_edit_icon_) {
    QLabel::setText(styled_text_.arg(text));
  } else {
    QLabel::setText(styled_text_.arg(text) + icon_text_.arg(icon_size_));
  }
  emit SignalTextUpdated();
  // QLabel::adjustSize();
}

void ClickableLabel::setFontSize(int size) {
  font_.setPointSize(size);
  QLabel::setFont(font_);
  icon_size_ = size;

  /*if (!hide_edit_icon_) {
      QLabel::setText(styled_text_.arg(text));
  } else {
      QLabel::setText(styled_text_.arg(text) + icon_text_.arg(icon_size_));
  }*/
  setText(text_);
}

QString ClickableLabel::text() const { return text_; }

void ClickableLabel::mouseReleaseEvent(QMouseEvent *) { emit clicked(); }

} // namespace FirstYear::UI
