#pragma once
#ifndef FIRSTYEAR_CORE_PROJECT_H
#define FIRSTYEAR_CORE_PROJECT_H
#include <QPixmap>
#include <QString>
#include <Types.h>
#include <optional>
#include <vector>

namespace FirstYear::Core {
struct MonthItem {
  std::optional<QPixmap> photo;
  int angle = 0;
  unsigned int scale = 1;
  std::pair<int, int> center_coordinates = {0, 0};
  std::optional<QString> text;
  QString filter_id_;
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
