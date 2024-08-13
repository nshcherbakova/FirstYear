
#include "FrameControl.h"
#include <Core/Project/FileSystemProjectLoader.h>
#include <Core/Project/FileSystemProjectWriter.h>
#include <Core/Project/Project.h>
#include <stdafx.h>

constexpr const char *DEF_PROGECT_NAME = "Frame";
namespace FirstYear::Core {

FrameControl::FrameControl() {}

ProjectPtr FrameControl::LoadProject() {
  if (auto name = LastProjectName(); !name.isEmpty()) {
    LoadProject(name);
  }
  if (!current_project_) {
    CreateNewProject();
  }

  return current_project_;
}

void FrameControl::SaveProject() {
  FileSystemProjectWriter().Write(current_project_);
}

void FrameControl::LoadProject(QString name) {
  current_project_ = FileSystemProjectLoader().Load(name);
}

ProjectPtr FrameControl::CurrentProject() { return current_project_; }

QString FrameControl::LastProjectName() const { return DEF_PROGECT_NAME; }

void FrameControl::CreateNewProject() {
  current_project_ = std::make_shared<Project>();
  current_project_->monthes_.resize(12);
}

} // namespace FirstYear::Core
