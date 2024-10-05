
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

  return current_project_;
}

void FrameControl::SaveProject() {
  FileSystemProjectWriter().Write(current_project_);
}

void FrameControl::SaveProjectMonth(int month) {
  FileSystemProjectWriter().Write(current_project_, month);
}

void FrameControl::LoadProject(QString name) {
  current_project_ = FileSystemProjectLoader().Load(name);

  for (int i = 0; i < (int)current_project_->monthes_.size(); i++) {
    auto &month = current_project_->monthes_[i];

    if (month.photo_data.image.isNull()) {
      month.photo_data.is_stub_image = true;
      month.photo_data.image =
          QPixmap(QString(c_stub_month_photo_template_str).arg(i));
      month.photo_data.angle = 0;
      month.photo_data.scale = 1;
      month.photo_data.offset = QPoint();
    }
  }
}

ProjectPtr FrameControl::CurrentProject() { return current_project_; }

QString FrameControl::LastProjectName() const { return DEF_PROGECT_NAME; }

void FrameControl::CreateNewProject() {
  current_project_ = std::make_shared<Project>();
  current_project_->monthes_.resize(12);

  for (int i = 0; i < (int)current_project_->monthes_.size(); i++) {
    auto &month = current_project_->monthes_[i];

    month.photo_data.is_stub_image = true;
    month.photo_data.image =
        QPixmap(QString(c_stub_month_photo_template_str).arg(i));
    month.photo_data.angle = 0;
    month.photo_data.scale = 1;
    month.photo_data.offset = QPoint();
  }
}
} // namespace FirstYear::Core
