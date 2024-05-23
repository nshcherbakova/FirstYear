#include "DefaultFrameWidget.h"
#include <stdafx.h>



namespace FirstYear::UI {
DefaultFrameWidget::DefaultFrameWidget(QWidget& parent)
    : QWidget(&parent) {


  setContentsMargins(0, 0, 0, 0);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  //palette.setColor(QPalette::Window, c_widget_background_color);
  setPalette(palette);


  //spdlog::info("FiltersWidget UI created");

  setEnabled(false);

}

void DefaultFrameWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    // Draw background

}



} // namespace FirstYear::UI
