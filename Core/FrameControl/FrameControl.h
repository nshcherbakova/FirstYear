#pragma once
#ifndef FIRSTYEAR_FRAME_CONTROL_H
#define FIRSTYEAR_FRAME_CONTROL_H
#include <QString>
#include <QWidget>
#include <Types.h>

namespace FirstYear::Core {

class FrameControl final {
public:
  explicit FrameControl();
  FrameControl &operator=(const FrameControl &) = delete;

  ProjectPtr LoadProject();
  void SaveProject();
  void SaveProjectMonth(int month);
  ProjectPtr CurrentProject();

private:
  void LoadProject(QString name);
  QString LastProjectName() const;
  void CreateNewProject();

private:
  ProjectPtr current_project_;
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_FRAME_CONTROL_H
