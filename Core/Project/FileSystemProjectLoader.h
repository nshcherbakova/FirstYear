#pragma once
#ifndef FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#define FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#include "Constants.h"
#include <QString>
#include <Types.h>

namespace FirstYear::Core {

class FileSystemProjectLoader : protected ProjectConstants {
public:
  ProjectPtr Load(QString name);

private:
  bool LoadMonth(int month_number, ProjectPtr &project);
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
