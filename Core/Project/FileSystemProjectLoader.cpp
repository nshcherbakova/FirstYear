#include "FileSystemProjectLoader.h"
#include <stdafx.h>

namespace FirstYear::Core {

ProjectPtr FileSystemProjectLoader::Load(QString /*name*/,
                                         const ImageManagerPtr &image_manager) {
  QFile project_metadata_file(project_metadata_path_);
  qDebug() << "Load project: " << project_metadata_path_;

  if (!project_metadata_file.open(QIODevice::ReadOnly)) {
    spdlog::info("Couldn't open file {0}.",
                 project_metadata_path_.toStdString());
    return nullptr;
  }

  auto project = std::make_shared<Project>();
  project->monthes_.resize(12);
  for (int month_number = 0; month_number < (int)project->monthes_.size();
       month_number++) {
    auto &month = project->monthes_[month_number];
    month.photo_data = std::make_shared<PhotoData>(image_manager);
  }

  const QByteArray project_metadata = project_metadata_file.readAll();

  const QJsonDocument project_json_document(
      QJsonDocument::fromJson(project_metadata));

  const auto project_json = project_json_document.object();

  if (!Core::Json::ReadString(project_json, "id", project->id_)) {
    spdlog::error("Error while reading a project id from json.");
    // return nullptr;
  }

  if (!Core::Json::ReadString(project_json, "title", project->title_)) {
    spdlog::error("Error while reading a project title from json.");
    // return nullptr;
  }

  if (!Core::Json::ReadString(project_json, "frame_id", project->frame_id_)) {
    spdlog::error("Error while reading a project frame id from json.");
    // return nullptr;
  }

  for (int i = 0; i < 12; i++) {
    if (!LoadMonth(i, project)) {
      spdlog::error("Error while loading a project month {0} data.", i);
    }
  }

  spdlog::info("Project loaded {0}.", project_data_path_.toStdString());
  return project;
}

bool FileSystemProjectLoader::LoadTransform(const QJsonObject &json,
                                            QString name,
                                            QTransform &transform) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;
    double m11 = 0;
    result = result && Core::Json::ReadDouble(object, "m11", m11);
    double m12 = 0;
    result = result && Core::Json::ReadDouble(object, "m12", m12);
    double m13 = 0;
    result = result && Core::Json::ReadDouble(object, "m13", m13);

    double m21 = 0;
    result = result && Core::Json::ReadDouble(object, "m21", m21);
    double m22 = 0;
    result = result && Core::Json::ReadDouble(object, "m22", m22);
    double m23 = 0;
    result = result && Core::Json::ReadDouble(object, "m23", m23);

    double m31 = 0;
    result = result && Core::Json::ReadDouble(object, "m31", m31);
    double m32 = 0;
    result = result && Core::Json::ReadDouble(object, "m32", m32);
    double m33 = 0;
    result = result && Core::Json::ReadDouble(object, "m33", m33);

    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
    return result;
  }
  return false;
}

bool FileSystemProjectLoader::LoadMonth(int month_number, ProjectPtr &project) {

  MonthItem &month = project->monthes_[month_number];

  QFile month_metadata_file(month_metadata_path_template_.arg(month_number));

  if (!month_metadata_file.open(QIODevice::ReadOnly)) {
    spdlog::error("Couldn't open file {0}.",
                  month_metadata_file.fileName().toStdString());
    //  return false;
  }

  const QByteArray month_metadata = month_metadata_file.readAll();
  month_metadata_file.close();

  const QJsonDocument month_json_document(
      QJsonDocument::fromJson(month_metadata));

  auto month_json = month_json_document.object();

  if (!LoadTransform(month_json, "transform_scale_rotate",
                     month.photo_data->transformScaleRotateRef())) {
    spdlog::error("Error while reading  transform_scale_rotate in a {0} month "
                  "photo scale from json.",
                  month_number);
    // return false;
  }
  if (!LoadTransform(month_json, "transform_offset",
                     month.photo_data->transformOffsetRef())) {
    spdlog::error("Error while reading transform_offset in a {0} month photo "
                  "scale from json.",
                  month_number);
    // return false;
  }

  if (!Core::Json::ReadString(month_json, "filter_id", month.filter_id)) {
    spdlog::error("Error while reading a {0} month photo filter_id from json",
                  month_number);
    // return false;
  }

  if (month_json.contains("text")) {
    QString text;
    if (!Core::Json::ReadString(month_json, "text", text)) {
      spdlog::error("Error while reading a {0} month photo text from json.",
                    month_number);
      // return false;
    } else {
      month.text = text;
    }
  }

  if (month_json.contains("photo_id")) {
    QString photo_id;
    if (!Core::Json::ReadString(month_json, "photo_id", photo_id)) {
      spdlog::error("Error while reading a {0} month photo text from json.",
                    month_number);
      // return false;
    } else {
      month.photo_data->setImageId(photo_id);
    }
  }

  return true;
}

} // namespace FirstYear::Core
