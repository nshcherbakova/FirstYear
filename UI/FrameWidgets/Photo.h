#pragma once
#ifndef FIRSTYEAR_UI_PHOTO_H
#define FIRSTYEAR_UI_PHOTO_H
#include <QPixmap>

namespace FirstYear::UI {
struct Photo {
  QPixmap image;
  double scale = 1;
  QPoint offset;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_PHOTO_H
