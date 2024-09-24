#pragma once
#ifndef FIRSTYEAR_FRAME_CONTROL_H
#define FIRSTYEAR_FRAME_CONTROL_H
#include <QString>
#include <QWidget>
#include <Types.h>
#include <map>

namespace FirstYear::Core {
class FrameControl final {
public:
  explicit FrameControl();
  FrameControl &operator=(const FrameControl &) = delete;

  ProjectPtr LoadProject();
  void SaveProject();
  void SaveProjectMonth(int month);
  ProjectPtr CurrentProject();

public:
  void CreateFrames(QWidget &parent);
  void ShowCurrentFrame();

public:
  void previousFrame();
  void nextFrame();
  bool isPreviousFrame();
  bool isNextFrame();

private:
  void LoadProject(QString name);
  QString LastProjectName() const;
  void CreateNewProject();

private:
  ProjectPtr current_project_;
  std::map<QString, UI::FrameWidgetBase *> frames_;
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_FRAME_CONTROL_H
