#pragma once
#ifndef FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
#define FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
#include <Types.h>

namespace FirstYear::Core {

class FileSystemProjectWriter {
public:
  static void Write(const ProjectPtr &project);
  static void Write(const ProjectPtr &project, int month);
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_CORE_FS_PROJECT_WRITER_H
