#include "InfoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

InfoWidget::InfoWidget(QWidget *parent, Core::FrameControl &frame_control)
    : QWidget(parent), frame_control_(frame_control) {
  setObjectName("Info");

  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  palette.setColor(QPalette::Window, c_info_text__background_color);
  setPalette(palette);

  text_ = new QLabel(this);
  text_->setObjectName("InfoText");
  text_->setText(QString("<h2>First Year</h2><br/><br/>"
                         "NShchApps<br/>"
                         "Version %1")
                     .arg(QCoreApplication::applicationVersion()));
  text_->setWordWrap(true);

  share_ = new TextButton(this);
  share_->setText("Share an errors report");
  share_->setSize(QSize(280, 70));
  share_->setObjectName("ShareLogButton");
  share_->setStyleSheet(c_share_log_button_style_str);

  connect(share_, &QPushButton::clicked, this, [&]() {
    emit SignalShareLog();
    hide();
  });
}

void InfoWidget::setVisible(bool visible) {
  if (visible) {
    QFileInfo log_file(frame_control_.logFilePath());
    share_->setVisible(log_file.size() > 0);
  }
  QWidget::setVisible(visible);
}

void InfoWidget::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);

  text_->setStyleSheet(QString(c_info_text_style_str).arg(height() / 30));

  text_->setGeometry({{(width() / 6) / 2, height() / 7},
                      QSize({(int)(width() * 4.0 / 6.0), height() / 2})});

  share_->setGeometry(
      {{text_->geometry().left() - 20, height() - 110}, share_->size()});
}

void InfoWidget::mouseReleaseEvent(QMouseEvent *e) {
  QWidget::mouseReleaseEvent(e);
  QTimer::singleShot(300, this, [&]() { hide(); });
}
} // namespace FirstYear::UI
