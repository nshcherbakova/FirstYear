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
  QTransform transform_scale_rotate;
  QTransform transform_offset;
};

struct MonthItem {
  PhotoData photo_data;
  std::optional<QString> text;
  QString filter_id;
};

class Project {
public:
  using MonthItems = std::vector<MonthItem>;

  QString id_; // project id
  QString title_ = "My First Year";
  QString frame_id_; // frame identificator
  MonthItems monthes_;
};

} // namespace FirstYear::Core
Q_DECLARE_METATYPE(FirstYear::Core::PhotoData);
#endif // FIRSTYEAR_CORE_PROJECT_H
