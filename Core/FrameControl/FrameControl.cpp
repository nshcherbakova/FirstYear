
#include "FrameControl.h"
#include <Core/Project/FileSystemProjectLoader.h>
#include <Core/Project/FileSystemProjectWriter.h>
#include <Core/Project/Project.h>
#include <stdafx.h>

constexpr const char *DEF_PROGECT_NAME = "Frame";
constexpr const char *c_stub_month_photo_template_str =
    ":images/month_stubs/month_stub_%1";

namespace FirstYear::Core {

FrameControl::FrameControl() {}

ProjectPtr FrameControl::LoadProject() {
  if (auto name = LastProjectName(); !name.isEmpty()) {
    LoadProject(name);
  }
  if (!current_project_) {
    CreateNewProject();
  }

  for (int i = 0; i < (int)current_project_->monthes_.size(); i++) {
    auto &month = current_project_->monthes_[i];

    month.stub_image_path = QString(c_stub_month_photo_template_str).arg(i);
    if (month.photo_data->image().isNull()) {
      month.photo_data->fillImage(QPixmap(month.stub_image_path), true, false);
    }
  }

  return current_project_;
}

void FrameControl::SaveProject() {
  FileSystemProjectWriter().Write(current_project_);
  spdlog::info("Project saved");
}

void FrameControl::SaveProjectMonth(int month) {
  FileSystemProjectWriter().Write(current_project_, month);
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
