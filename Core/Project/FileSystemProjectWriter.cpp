#include "FileSystemProjectWriter.h"
#include "Project.h"
#include <stdafx.h>

constexpr const char *PROJECT_NAME = "FirstYear";
constexpr const char *METADAT_NAME = "metadata";
namespace FirstYear::Core {

bool CheckExistingProject() {
  auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  return QDir().exists(path + PROJECT_NAME);
}

bool CreateProject() {
  auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir project_dir(path + PROJECT_NAME);
  UNI_ENSURE_RETURN(!project_dir.exists(), false);
  QDir().mkpath(path + PROJECT_NAME);

  QFile projetct_metadata(path + PROJECT_NAME + "//" + METADAT_NAME);
  if (!projetct_metadata.open(QIODevice::WriteOnly)) {
    spdlog::error("Error while creating file {0}",
                  projetct_metadata.fileName().toStdString());
    return false;
  }

  for (int i = 1; i <= 12; i++) {
    QString month_path = path + PROJECT_NAME + "//" + QString::number(i);
    QDir().mkpath(month_path);

    QFile month_metadata(month_path + "//" + METADAT_NAME);
    if (!month_metadata.open(QIODevice::WriteOnly)) {
      spdlog::error("Error while creating file {0}",
                    projetct_metadata.fileName().toStdString());
    }
  }

  return true;
}

/*
struct MonthItem {
  std::optional<QPixmap> photo;
  int angle = 0;
  unsigned int scale = 1;
  std::pair<int, int> center_coordinates = {0, 0};
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

  const auto path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  QFile projetct_metadata_file(path + PROJECT_NAME + "//" + METADAT_NAME);
  projetct_metadata_file.open(QIODevice::WriteOnly);

  projetct_metadata_file.write(project_metadata_document.toJson());
  projetct_metadata_file.close();

  for (int i = 0; i < 12; i++) {
    QJsonObject month_metadata;
    month_metadata.insert("angle", project->monthes_[i].angle);
    month_metadata.insert("scale", project->monthes_[i].scale);

    QJsonObject point;
    point.insert("x", project->monthes_[i].center_coordinates.x());
    point.insert("y", project->monthes_[i].center_coordinates.y());

    month_metadata.insert("center_coordinates", point);

    if (project->monthes_[i].text) {
      month_metadata.insert("text", *project->monthes_[i].text);
    }
    month_metadata.insert("filter_id_", project->monthes_[i].filter_id_);

    QJsonDocument month_metadata_document(project_metadata);

    const QString month_path =
        path + PROJECT_NAME + "//" + QString::number(i + 1);

    QFile month_metadata_file(month_path + "//" + METADAT_NAME);
    month_metadata_file.open(QIODevice::WriteOnly);

    month_metadata_file.write(month_metadata_document.toJson());
    month_metadata_file.close();
  }
}

} // namespace FirstYear::Core
