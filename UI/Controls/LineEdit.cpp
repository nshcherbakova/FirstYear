#include "LineEdit.h"
#include <stdafx.h>

static const int c_max_month_lengh = 30;

namespace FirstYear::UI {

LineEditWidget::LineEditWidget(QWidget *parent)
    : QWidget(parent), line_edit_(new QLineEdit(this)) {
  line_edit_->setMaxLength(c_max_month_lengh);
  line_edit_->setStyleSheet(c_line_edit_style_str);
  line_edit_->setTextMargins(QMargins(10, 10, 10, 10));
  line_edit_->setAlignment(Qt::AlignCenter);

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
    QInputMethod *keyboard = QGuiApplication::inputMethod();
    keyboard->show();
  }
}

void LineEditWidget::setText(QString text, int id) {
  id_ = id;
  line_edit_->setText(text);
  //  spdlog::info("LineEditWidget {}", text.toStdString());
}

void LineEditWidget::resizeEvent(QResizeEvent *e) {
  if (!e->size().isValid() || e->size().isEmpty()) {
    return;
  }
  QWidget::resizeEvent(e);

  //   QInputMethod *keyboard = QGuiApplication::inputMethod();
  //  qDebug() << "**** " << keyboard->isVisible();
  /* if(keyboard->isVisible())
     {

         int size = std::min(width(), height());
         int line_edit_width = (int)(width() * 0.8);
         QRect rect = {(width() - line_edit_width) / 2, (int)(height() / 8),
                       line_edit_width, (int)(size / 2)};

         line_edit_->setGeometry({0,0, 20, 400});
         QFont font = line_edit_->font();
         font.setPointSize(20);
          qDebug() << "****1 " << size / 4.0;
         line_edit_->setFont(font);
   }
   else*/
  {

    int size = std::min(width(), height());
    bool portrait = width() < height();
    int line_edit_width = (int)(width() * 0.8);
    QRect rect = {(width() - line_edit_width) / 2,
                  portrait ? (int)(height() / 3) : (int)(height() / 8),
                  line_edit_width, (int)(size / 4.5)};

    line_edit_->setGeometry(rect);
    QFont font = line_edit_->font();

    font.setPointSize(size / 10.0);

    line_edit_->setFont(font);
  }
}

void LineEditWidget::mouseReleaseEvent(QMouseEvent *e) {
  QWidget::mouseReleaseEvent(e);

  emit SignalTextChanged(line_edit_->text(), id_);
  hide();
}
} // namespace FirstYear::UI
