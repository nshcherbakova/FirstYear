#include "FileSystemProjectWriter.h"
#include <functional>
#include <stdafx.h>

namespace FirstYear::Core {

bool saveFile(QString path, std::function<bool(QString namec)> saveFunction) {
  const auto path_tmp = path + "_tmp";
  const auto path_old = path + "_old";

  qDebug() << "Save " << path;
  if (QFileInfo::exists(path_tmp)) {
    QFile::remove(path_tmp);
  }

  if (QFileInfo::exists(path_old)) {
    QFile::remove(path_old);
  }

  if (!saveFunction(path_tmp)) {
    spdlog::info("Error, image was not saved {0}", path_tmp.toStdString());
    return false;
  } else if (!QFileInfo::exists(path)) {
    return QFile::rename(path_tmp, path);
  } else {

    if (!QFile::rename(path, path_old)) {
      return false;
    }

    if (QFile::rename(path_tmp, path)) {
      QFile::remove(path_old);
      return true;
    } else {
      spdlog::info("Error, image was not renamed {0}", path_tmp.toStdString());
      // rename back;
      QFile::rename(path_old, path);
      QFile::remove(path_tmp);
      return false;
    }
  }
}

bool FileSystemProjectWriter::CheckExistingProject() {
  return QDir().exists(project_metadata_path_);
}

bool FileSystemProjectWriter::CreateProjectFiles() {

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

  if (project->state != 0) {
    project->state = 0;
    QJsonObject project_metadata;
    project_metadata.insert("id", project->id_);
    project_metadata.insert("title", project->title_);
    project_metadata.insert("frame_id", project->frame_id_);

    QJsonDocument project_metadata_document(project_metadata);

    const bool result =
        saveFile(project_metadata_path_, [&](QString path) -> bool {
          QFile file(path);
          if (!file.open(QIODevice::WriteOnly)) {
            return false;
          }
          const auto data = project_metadata_document.toJson();
          const auto res_size = file.write(data);
          if (res_size != data.size()) {
            return false;
          }
          file.close();
          return true;
        });

    if (!result) {
      spdlog::error("Can't save {} project metadata.",
                    project_metadata_path_.toStdString());
    }

    spdlog::info("Project data saved");
  }

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

  if (month_data.photo_data->state() & (short)PhotoData::STATE::CHANGED ||
      month_data.state & (short)MonthItem::STATE::CHANGED) {
    QJsonObject month_metadata;

    month_metadata.insert(
        "transform_offset",
        TransformJson(month_data.photo_data->transformOffset()));
    month_metadata.insert(
        "transform_scale_rotate",
        TransformJson(month_data.photo_data->transformScaleRotate()));

    month_metadata.insert("text", month_data.text);

    month_metadata.insert("filter_id", month_data.filter_id);

    month_metadata.insert("photo_id", month_data.photo_data->imageId());

    QJsonDocument month_metadata_document(month_metadata);

    const QString month_metadata_path =
        month_metadata_path_template_.arg(QString::number(month));

    const bool result =
        saveFile(month_metadata_path, [&](QString path) -> bool {
          QFile file(path);
          if (!file.open(QIODevice::WriteOnly)) {
            return false;
          }
          const auto data = month_metadata_document.toJson();
          const auto res_size = file.write(month_metadata_document.toJson());
          if (res_size != data.size()) {
            return false;
          }
          file.close();
          return true;
        });

    if (!result) {
      spdlog::error("Can't save {} metadata.",
                    month_metadata_path.toStdString());
    }
    spdlog::info("Month {} data saved", month);
  }

  month_data.photo_data->setState(0);
  month_data.state = 0;
}
} // namespace FirstYear::Core
