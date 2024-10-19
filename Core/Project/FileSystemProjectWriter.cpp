#include "FileSystemProjectWriter.h"
#include <stdafx.h>

namespace FirstYear::Core {

bool FileSystemProjectWriter::CheckExistingProject() {
  return QDir().exists(project_data_path_);
}

bool FileSystemProjectWriter::CreateProjectFiles() {

  QDir project_dir(project_data_path_);
  UNI_ENSURE_RETURN(!project_dir.exists(), false);
  if (!QDir().mkpath(project_data_path_)) {
    spdlog::error("Error while creating directory {0}",
                  project_data_path_.toStdString());
    return false;
  }

  QFile projetct_metadata(project_metadata_path_);
  if (!projetct_metadata.open(QIODevice::WriteOnly)) {
    spdlog::error("Error while creating file {0}",
                  projetct_metadata.fileName().toStdString());
    return false;
  }

  for (int i = 0; i < 12; i++) {
    QString month_path = month_path_template_.arg(QString::number(i));
    QDir().mkpath(month_path);

    QFile month_metadata(month_metadata_path_template_.arg(QString::number(i)));
    if (!month_metadata.open(QIODevice::WriteOnly)) {
      spdlog::error("Error while creating file {0}",
                    month_metadata.fileName().toStdString());
    }
  }

  return true;
}

void FileSystemProjectWriter::Write(const ProjectPtr &project) {
  spdlog::debug("Save project to {0}", project_data_path_.toStdString());
  UNI_ASSERT(project);

  if (!CheckExistingProject()) {
    auto status = CreateProjectFiles();
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

QJsonObject
FileSystemProjectWriter::TransformJson(const QTransform &transform) {
  QJsonObject transform_data;
  transform_data.insert("m11", transform.m11());
  transform_data.insert("m12", transform.m12());
  transform_data.insert("m13", transform.m13());

  transform_data.insert("m21", transform.m21());
  transform_data.insert("m22", transform.m22());
  transform_data.insert("m23", transform.m23());

  transform_data.insert("m31", transform.m31());
  transform_data.insert("m32", transform.m32());
  transform_data.insert("m33", transform.m33());

  return transform_data;
}

void FileSystemProjectWriter::Write(const ProjectPtr &project, int month) {
  UNI_ASSERT(project);
  UNI_ASSERT(month >= 0 && month < 12);

  const auto &month_data = project->monthes_[month];

  QJsonObject month_metadata;

  month_metadata.insert("transform_offset",
                        TransformJson(month_data.photo_data.transform_offset));
  month_metadata.insert(
      "transform_scale_rotate",
      TransformJson(month_data.photo_data.transform_scale_rotate));

  if (month_data.text) {
    month_metadata.insert("text", *month_data.text);
  }
  month_metadata.insert("filter_id", month_data.filter_id);

  QJsonDocument month_metadata_document(month_metadata);

  const QString month_path = month_path_template_.arg(QString::number(month));

  const QString month_metadata_path =
      month_metadata_path_template_.arg(QString::number(month));
  QFile month_metadata_file(month_metadata_path);
  if (!month_metadata_file.open(QIODevice::WriteOnly)) {
    spdlog::error("Can't open {} to save metadata.",
                  month_metadata_path.toStdString());
  }
  month_metadata_file.write(month_metadata_document.toJson());
  month_metadata_file.close();

  if (!month_data.photo_data.is_stub_image) {
    if (!month_data.photo_data.image.isNull()) {
      if (!month_data.photo_data.image.save(
              month_photo_path_template_.arg(month), IMAGE_FORMAT)) {
        spdlog::info("Error, image was not saved {0}",
                     month_photo_path_template_.arg(month).toStdString());
      }
    }
  }
}
} // namespace FirstYear::Core
