#include "FileSystemProjectLoader.h"
#include <stdafx.h>

namespace FirstYear::Core {
/*
struct MonthItem {
    std::optional<QPixmap> photo;
    int angle = 0;
    int scale = 1;
    QPoint center_coordinates = {0, 0};
    std::optional<QString> text;
    QString filter_id_;
};

class Project {
public:
    using MonthItems = std::vector<MonthItem>;

    QString id_;
    QString title_ = "First year";
    QString frame_id_;
    MonthItems monthes_;
};
*/
bool ReadString(const QJsonObject &json, QString key, QString &value) {
  if (const QJsonValue json_value = json[key]; json_value.isString()) {
    value = json_value.toString();
    return true;
  }

  spdlog::error("Error while reading a string from json.");
  return false;
}

bool ReadInt(const QJsonObject &json, QString key, int &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toInt();
    return true;
  }

  spdlog::error("Error while reading an int from json.");
  return false;
}

bool ReadDouble(const QJsonObject &json, QString key, double &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toDouble();
    return true;
  }

  spdlog::error("Error while reading a double from json.");
  return false;
}
ProjectPtr FileSystemProjectLoader::Load(QString /*name*/) {
  auto project = std::make_shared<Project>();
  project->monthes_.resize(12);

  QFile project_metadata_file(project_metadata_path_);

  if (!project_metadata_file.open(QIODevice::ReadOnly)) {
    spdlog::info("Couldn't open file {0}.",
                 project_metadata_path_.toStdString());
    return nullptr;
  }

  const QByteArray project_metadata = project_metadata_file.readAll();

  const QJsonDocument project_json_document(
      QJsonDocument::fromJson(project_metadata));

  const auto project_json = project_json_document.object();

  if (!ReadString(project_json, "id", project->id_)) {
    spdlog::error("Error while reading a project id from json {0}.",
                  project_metadata.toStdString());
    return nullptr;
  }

  if (!ReadString(project_json, "title", project->title_)) {
    spdlog::error("Error while reading a project title from json {0}.",
                  project_metadata.toStdString());
    return nullptr;
  }

  if (!ReadString(project_json, "frame_id", project->frame_id_)) {
    spdlog::error("Error while reading a project frame id from json {0}.",
                  project_metadata.toStdString());
    return nullptr;
  }

  for (int i = 0; i < 12; i++) {
    if (!LoadMonth(i, project)) {
      return nullptr;
    }
  }
  return project;
}
/*
std::optional<QPixmap> photo;
int angle = 0;
int scale = 1;
QPoint center_coordinates = {0, 0};
std::optional<QString> text;
QString filter_id;
*/
bool FileSystemProjectLoader::LoadMonth(int month_number, ProjectPtr &project) {

  MonthItem &month = project->monthes_[month_number];
  QFile month_metadata_file(month_metadata_path_template_.arg(month_number));

  if (!month_metadata_file.open(QIODevice::ReadOnly)) {
    spdlog::error(
        "Couldn't open file {0}.",
        month_metadata_path_template_.arg(month_number).toStdString());
    return false;
  }

  const QByteArray month_metadata = month_metadata_file.readAll();

  const QJsonDocument month_json_document(
      QJsonDocument::fromJson(month_metadata));

  auto month_json = month_json_document.object();

  if (!ReadDouble(month_json, "angle", month.photo_data.angle)) {
    spdlog::error("Error while reading a {0} month photo angle from json {1}.",
                  month_number, month_metadata.toStdString());
    return false;
  }

  if (!ReadDouble(month_json, "scale", month.photo_data.scale)) {
    spdlog::error("Error while reading a {0} month photo scale from json {1}.",
                  month_number, month_metadata.toStdString());
    return false;
  }

  if (!ReadString(month_json, "filter_id", month.filter_id)) {
    spdlog::error(
        "Error while reading a {0} month photo filter_id from json {1}.",
        month_number, month_metadata.toStdString());
    return false;
  }

  if (month_json.contains("text")) {
    QString text;
    if (!ReadString(month_json, "text", text)) {
      spdlog::error("Error while reading a {0} month photo text from json {1}.",
                    month_number, month_metadata.toStdString());
      return false;
    }
    month.text = text;
  }

  if (const auto offset = month_json["offset"]; offset.isObject()) {
    const auto offset_object = offset.toObject();
    int x = 0, y = 0;

    if (!ReadInt(offset_object, "x", x)) {
      spdlog::error(
          "Error while reading a {0} month photo coordinate x from json {1}.",
          month_number, month_metadata.toStdString());
      return false;
    }
    if (!ReadInt(offset_object, "y", y)) {
      spdlog::error(
          "Error while reading a {0} month photo coordinate x from json {1}.",
          month_number, month_metadata.toStdString());
      return false;
    }
    month.photo_data.offset = {x, y};
  }

  QPixmap photo(month_photo_path_template_.arg(month_number));
  if (!photo.isNull()) {
    month.photo_data.image = photo;
  }
  return true;
}

} // namespace FirstYear::Core
