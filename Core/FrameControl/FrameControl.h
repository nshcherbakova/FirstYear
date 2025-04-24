#pragma once
#ifndef FIRSTYEAR_FRAME_CONTROL_H
#define FIRSTYEAR_FRAME_CONTROL_H
#include <Core/Project/Constants.h>
#include <QString>
#include <QWidget>
#include <Types.h>
#include <unordered_map>

namespace FirstYear::Core {

class FrameControl final {
public:
  explicit FrameControl();
  FrameControl &operator=(const FrameControl &) = delete;

  ProjectPtr LoadProject();
  void SaveProject();
  void SaveProjectMonth(int month);
  ProjectPtr CurrentProject();
  ImageManagerPtr imageManager();

private:
  void LoadProject(QString name);
  QString LastProjectName() const;
  void CreateNewProject();

private:
  ProjectPtr current_project_;
  ImageManagerPtr image_manager_;
};

class ImageManager : private ProjectConstants {
public:
  ImageManager() {}

public:
  void loadImages(int month_count);
  const QPixmap &image(QString image_name) const;

  bool isStub(QString image_name) const;

  QString addImage(QString image_path);
  void deleteImage(QString image_path);

private:
  void fillImage(QString image_path, QString image_name);
  void fillStubImage(QString image_name);

private:
  std::unordered_map<QString, QPixmap> images_;
  QPixmap empty_;
};

} // namespace FirstYear::Core
#endif // FIRSTYEAR_FRAME_CONTROL_H
