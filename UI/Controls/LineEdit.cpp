#include "LineEdit.h"
#include <stdafx.h>

static const int c_max_month_lengh = 20;

namespace FirstYear::UI {

LineEditWidget::LineEditWidget(QWidget *parent)
    : QWidget(parent), line_edit_(new QLineEdit(this)) {
  line_edit_->setMaxLength(c_max_month_lengh);
  line_edit_->setStyleSheet(c_line_edit_style_str);

  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  palette.setColor(QPalette::Window, c_line_edit_background_color);
  setPalette(palette);

  connect(line_edit_, &QLineEdit::returnPressed, this, [&] {
    emit SignalTextChanged(line_edit_->text(), id_);
    hide();
  });
}

void LineEditWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  if (!visible) {
    id_ = -1;
    line_edit_->setText("");
  } else {
    line_edit_->setFocus();
  }
}

void LineEditWidget::setText(QString text, int id) {
  id_ = id;
  line_edit_->setText(text);
  spdlog::error("LineEditWidget {}", text.toStdString());
}

void LineEditWidget::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);

  int size = std::min(width(), height());
  int line_edit_width = (int)(size * 0.8);
  QRect rect = {(width() - line_edit_width) / 2, (int)(height() / 3),
                line_edit_width, (int)(size / 4.5)};

  line_edit_->setGeometry(rect);
  QFont font = line_edit_->font();
  font.setPointSize(size / 10);
  line_edit_->setFont(font);
}

void LineEditWidget::mouseReleaseEvent(QMouseEvent *) {

  emit SignalTextChanged(line_edit_->text(), id_);
  hide();
}
} // namespace FirstYear::UI
