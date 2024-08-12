#pragma once
#ifndef FIRSTYEAR_FRAME_CONTROL_H
#define FIRSTYEAR_FRAME_CONTROL_H
#include <QString>
#include <Types.h>

namespace FirstYear::Core {
class FrameControl final {
public:
  explicit FrameControl();
  FrameControl &operator=(const FrameControl &) = delete;

  ProjectPtr LoadProject();
  void SaveProject();
  ProjectPtr CurrentProject();

public: // IWidget
        // virtual void onShow(const bool visible) override final;
  // signals:

  // public slots:

private:
  void LoadProject(QString name);
  QString LastProjectName() const;
  void CreateNewProject();

private:
  ProjectPtr current_project_;
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_FRAME_CONTROL_H
