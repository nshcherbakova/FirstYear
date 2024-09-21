#pragma once
#ifndef FIRSTYEAR_CORE_PROJECT_H
#define FIRSTYEAR_CORE_PROJECT_H
#include <QPixmap>
#include <QRectF>
#include <QString>
#include <Types.h>
#include <optional>
#include <vector>

namespace FirstYear::Core {
struct PhotoData {
  QPixmap image;
  bool is_stub_image = false;
  double angle = 0;
  double scale = 1;
  QPointF offset;
  //  QRectF dest_rect;
};

struct MonthItem {
  PhotoData photo_data;
  std::optional<QString> text;
  QString filter_id;
};

class Project {
public:
  using MonthItems = std::vector<MonthItem>;

  QString id_;
  QString title_ = "First year";
  QString frame_id_;
  MonthItems monthes_;
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_PROJECT_H
