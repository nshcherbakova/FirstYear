#pragma once
#ifndef FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#define FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#include "Constants.h"
#include <QJsonObject>
#include <QString>
#include <QTransform>
#include <Types.h>

namespace FirstYear::Core {

class FileSystemProjectLoader : protected ProjectConstants {
public:
  ProjectPtr Load(QString name, const ImageManagerPtr &image_manager);

private:
  bool LoadMonth(int month_number, ProjectPtr &project);

  bool LoadTransform(const QJsonObject &json, QString name,
                     QTransform &transform);
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
