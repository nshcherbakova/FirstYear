#pragma once
#ifndef FIRSTYEAR_FRAME_CONTROL_H
#define FIRSTYEAR_FRAME_CONTROL_H
#include <Core/Project/Constants.h>
#include <QObject>
#include <QString>
#include <QWidget>
#include <Types.h>
#include <unordered_map>

namespace FirstYear::Core {
class Context {
public:
  Context(const QString &locale);

public:
  QString locale() const;

private:
  const QString locale_;
};

struct FrameControlParameters {
  QObject *parent;
  const QString log_file_path;
  const QString locale;
};

class FrameControl : public QObject, private ProjectConstants {
  Q_OBJECT
public:
  explicit FrameControl(FrameControlParameters parameters);
  FrameControl &operator=(const FrameControl &) = delete;

  ProjectPtr LoadProject();
  void SaveProject();
  void SaveProjectMonth(int month);
  ProjectPtr CurrentProject();
  ImageManagerPtr imageManager();

  QString logFilePath() const;
  const Context &context();

private:
  void LoadProject(QString name);
  QString LastProjectName() const;
  void CreateNewProject();

private:
  ProjectPtr current_project_;
  ImageManagerPtr image_manager_;

  QTimer *save_timer_ = nullptr;
  QString log_file_path_;
  QStringList monthes_text_;
  Context context_;
};

class ImageManager : private ProjectConstants {
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
