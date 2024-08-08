#pragma once
#ifndef FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#define FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
#include <QString>
#include <Types.h>

namespace FirstYear::Core {

class FileSystemProjectLoader {
public:
  static ProjectPtr Load(QString name);
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_FS_PROJECT_LOADER_H
