#pragma once
#ifndef FIRSTYEAR_CORE_CONSTANTS_H
#define FIRSTYEAR_CORE_CONSTANTS_H

#include <QStandardPaths>

namespace FirstYear::Core {
struct ProjectConstants {
  const char *METADAT_NAME = "metadata";
  const char *IMAGE_FORMAT = "JPG";

  const QString project_path_ =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  const QString project_data_path_ = project_path_ + "/" + "data";
  const QString project_metadata_path_ =
      project_data_path_ + "/" + METADAT_NAME;
  const QString month_path_template_ = project_data_path_ + "/%1";
  const QString month_metadata_path_template_ =
      project_data_path_ + "/%1/" + METADAT_NAME;
  const QString month_photo_path_template_ = month_path_template_ + "/photo";
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_CONSTANTS_H
