
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
}

ProjectPtr FrameControl::CurrentProject() { return current_project_; }

QString FrameControl::LastProjectName() const { return DEF_PROGECT_NAME; }

void FrameControl::CreateNewProject() {
  current_project_ = std::make_shared<Project>();
  current_project_->monthes_.resize(12);

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
    // month.photo_data.dest_rect = photo_slots_[i];
    // spdlog::info("DefaultFrameWidget dest_rect width = {}",
    // month.photo_data.dest_rect.size().width());
  }
}

/*
void FrameControl::CreateFrames(QWidget &parent) {
  std::vector<UI::FrameWidgetBase *> frames(
      {new UI::DefaultFrameWidget(parent, *this),
       new UI::DefaultFrameWidget2(parent, *this)});

  for (auto &frame : frames) {

    frames_[frame->id()] = frame;
    frame->hide();
  }



}

void FrameControl::ShowCurrentFrame() {
  if (current_project_->frame_id_.isEmpty()) {
    current_project_->frame_id_ = frames_.begin()->first;
  }
  frames_[current_project_->frame_id_]->load(*this);
}

void FrameControl::previousFrame() {
  if (isPreviousFrame()) {
    frames_[current_project_->frame_id_]->hide();
    auto it = frames_.find(current_project_->frame_id_);

    auto frame_to_show = std::prev(it)->second;
    current_project_->frame_id_ = frame_to_show->id();
    frame_to_show->load(*this);
  }
}
void FrameControl::nextFrame() {
  if (isNextFrame()) {
    frames_[current_project_->frame_id_]->hide();
    auto it = frames_.find(current_project_->frame_id_);

    auto frame_to_show = std::next(it)->second;
    current_project_->frame_id_ = frame_to_show->id();
    frame_to_show->load(*this);
  }
}

bool FrameControl::isPreviousFrame() {
    return false;
  auto it = frames_.find(current_project_->frame_id_);
  if (it == frames_.begin())
    return false;
  else
    return true;
}

bool FrameControl::isNextFrame() {
    return false;
  auto it = frames_.find(current_project_->frame_id_);
  if (std::next(it) == frames_.end())
    return false;
  else
    return true;
}*/
} // namespace FirstYear::Core
