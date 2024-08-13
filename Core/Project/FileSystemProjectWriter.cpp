#include "FileSystemProjectWriter.h"
#include "Project.h"
#include <stdafx.h>

constexpr const char *PROJECT_NAME = "FirstYear";
constexpr const char *METADAT_NAME = "metadata";
constexpr const char *IMAGE_FORMAT = "JPG";
namespace FirstYear::Core {

QString project_path_ =
    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
    PROJECT_NAME;
QString project_metadata_path_ = project_path_ + "//" + METADAT_NAME;
QString month_path_template_ = project_path_ + "//%1";
QString month_metadata_path_template_ = project_path_ + "//%1//" + METADAT_NAME;
QString month_photo_path_template_ = month_path_template_ + "//photo";

bool CheckExistingProject() { return QDir().exists(project_path_); }

bool CreateProject() {
  QDir project_dir(project_path_);
  UNI_ENSURE_RETURN(!project_dir.exists(), false);
  QDir().mkpath(project_path_);

  QFile projetct_metadata(project_path_ + "//" + METADAT_NAME);
  if (!projetct_metadata.open(QIODevice::WriteOnly)) {
    spdlog::error("Error while creating file {0}",
                  projetct_metadata.fileName().toStdString());
    return false;
  }

  for (int i = 0; i < 12; i++) {
    QString month_path = project_path_ + "//" + QString::number(i);
    QDir().mkpath(month_path);

    QFile month_metadata(month_path + "//" + METADAT_NAME);
    if (!month_metadata.open(QIODevice::WriteOnly)) {
      spdlog::error("Error while creating file {0}",
                    projetct_metadata.fileName().toStdString());
    }
  }

  return true;
}

void FileSystemProjectWriter::Write(const ProjectPtr &project) {
  UNI_ASSERT(project);
  if (!CheckExistingProject()) {
    auto status = CreateProject();
    UNI_ENSURE_RETURN(status);
  }

  QJsonObject project_metadata;
  project_metadata.insert("id", project->id_);
  project_metadata.insert("title", project->title_);
  project_metadata.insert("frame_id", project->frame_id_);

  QJsonDocument project_metadata_document(project_metadata);

  QFile projetct_metadata_file(project_metadata_path_);
  projetct_metadata_file.open(QIODevice::WriteOnly);

  projetct_metadata_file.write(project_metadata_document.toJson());
  projetct_metadata_file.close();

  for (int i = 0; i < 12; i++) {
    Write(project, i);
  }
}

void FileSystemProjectWriter::Write(const ProjectPtr &project, int month) {
  UNI_ASSERT(project);
  UNI_ASSERT(month >= 0 && month < 12);

  QJsonObject month_metadata;
  month_metadata.insert("angle", project->monthes_[month].angle);
  month_metadata.insert("scale", project->monthes_[month].scale);

  QJsonObject point;
  point.insert("x", project->monthes_[month].center_coordinates.x());
  point.insert("y", project->monthes_[month].center_coordinates.y());

  month_metadata.insert("center_coordinates", point);

  if (project->monthes_[month].text) {
    month_metadata.insert("text", *project->monthes_[month].text);
  }
  month_metadata.insert("filter_id_", project->monthes_[month].filter_id);

  QJsonDocument month_metadata_document(month_metadata);

  const QString month_path = month_path_template_.arg(QString::number(month));

  QFile month_metadata_file(
      month_metadata_path_template_.arg(QString::number(month)));
  month_metadata_file.open(QIODevice::WriteOnly);

  month_metadata_file.write(month_metadata_document.toJson());
  month_metadata_file.close();

  if (project->monthes_[month].photo) {
    if (!project->monthes_[month].photo->save(
            month_photo_path_template_.arg(month), IMAGE_FORMAT)) {
      spdlog::info("Error, image was not saved {0}",
                   month_photo_path_template_.arg(month).toStdString());
    }
  }
}
} // namespace FirstYear::Core
