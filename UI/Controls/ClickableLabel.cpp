#include "ClickableLabel.h"
#include <stdafx.h>

namespace FirstYear::UI {

ClickableLabel::ClickableLabel(QWidget *parent, int font_size,
                               QString font_color, QString font_family,
                               bool hide_edit_icon, bool ajust_text)
    : QLabel(parent), hide_edit_icon_(hide_edit_icon), ajust_text_(ajust_text) {

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

  updateFontSize();
}

void ClickableLabel::updateFontSize() {
  if (width() == 0) {
    return;
  }

  if (!ajust_text_) {
    return;
  }

  if (text_.isEmpty()) {
    return;
  }

  QLabel::setFont(font_);
  qDebug() << icon_size_;
  qDebug() << "font size " << font_.pointSize();
  qDebug() << "width() " << width();

  const double new_with = sizeHint().width();
  qDebug() << "sizeHint() " << new_with;

  qDebug() << "  ";
  if (new_with > width()) {
    const auto koef = new_with / width();
    auto font = QLabel::font();
    const auto new_font_size = font.pointSize() / koef;

    font.setPointSize(new_font_size);
    QLabel::setFont(font);
  }
}

void ClickableLabel::setFontSize(int size) {
  font_.setPointSize(size);
  QLabel::setFont(font_);
  icon_size_ = size;

  if (!hide_edit_icon_) {
    QLabel::setText(styled_text_.arg(text_) + icon_text_.arg(icon_size_));
  }

  updateFontSize();
}

QString ClickableLabel::text() const { return text_; }

void ClickableLabel::mouseReleaseEvent(QMouseEvent *) { emit clicked(); }

void ClickableLabel::resizeEvent(QResizeEvent *event) {
  QLabel::resizeEvent(event);
  updateFontSize();
}

} // namespace FirstYear::UI
