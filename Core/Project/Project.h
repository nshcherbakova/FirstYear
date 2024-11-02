#pragma once
#ifndef FIRSTYEAR_CORE_PROJECT_H
#define FIRSTYEAR_CORE_PROJECT_H
#include <QPixmap>
#include <QRectF>
#include <QString>
#include <Types.h>
#include <vector>

namespace FirstYear::Core {
struct PhotoData {
  QPixmap image;
  bool is_stub_image = false;
  QTransform transform_scale_rotate;
  QTransform transform_offset;

  mutable short state = 0;
  enum class STATE : short {
    IMAGE_CHANGED = 0x01,
    TRANSFORM_SR_CHANGED = 0x02,
    TRANSFORM_OFFSET_CHANGED = 0x04,
  };
};

struct MonthItem {
  PhotoData photo_data;
  QString text;
  QString filter_id;

  mutable short state = 0;
  enum class STATE : short {
    //  PHOTO_DATA_CHANGED = 0x01,
    TEXT_CHANGED = 0x01,
    FILTER_ID_CHANGED = 0x02,
  };
};

class Project {
public:
  using MonthItems = std::vector<MonthItem>;

  QString id_; // project id
  QString title_ = "My First Year";
  QString frame_id_; // frame identificator
  MonthItems monthes_;

  // bool changed = false;
  mutable short state = 0;
  enum class STATE : short {
    ID_CHANGED = 0x01,
    TITLE_CHANGED = 0x02,
    FRAME_ID_CHANGED = 0x04,
  };
};

} // namespace FirstYear::Core
Q_DECLARE_METATYPE(FirstYear::Core::PhotoData);
#endif // FIRSTYEAR_CORE_PROJECT_H
