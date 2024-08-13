#pragma once
#ifndef FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
#define FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
#include "Constants.h"
#include <Types.h>

namespace FirstYear::Core {

class FileSystemProjectWriter : protected ProjectConstants {
public:
  void Write(const ProjectPtr &project);
  void Write(const ProjectPtr &project, int month);

private:
  bool CheckExistingProject();
  bool CreateProjectFiles();
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
