
#include "FrameControl.h"
#include <Core/Project/Constants.h>
#include <Core/Project/FileSystemProjectLoader.h>
#include <Core/Project/FileSystemProjectWriter.h>
#include <stdafx.h>

constexpr const char *DEF_PROGECT_NAME = "Frame";
constexpr const char *c_stub_month_photo_template_str =
    ":images/month_stubs/month_stub_%1";
static const QStringList monthes = {"one",  "two", "three",  "four",
                                    "five", "six", "seven",  "eight",
                                    "nine", "ten", "eleven", "twelve"};
constexpr const char *c_photo_prefix_str = "photo";
constexpr const int c_save_timeout = 20;

namespace FirstYear::Core {

FrameControl::FrameControl(QObject *parent)
    : QObject(parent), image_manager_(std::make_shared<ImageManager>()),
      save_timer_(new QTimer(this)) {
  PhotoData::image_manager_ = image_manager_;
  save_timer_->setInterval(c_save_timeout * 1000);
  connect(save_timer_, &QTimer::timeout, this, [&]() {
    FileSystemProjectWriter().Write(current_project_);

    for (int i = 0; i < monthes.size(); i++) {
      FileSystemProjectWriter().Write(current_project_, i);
    };
    save_timer_->start();
  });
}

ProjectPtr FrameControl::LoadProject() {
  if (auto name = LastProjectName(); !name.isEmpty()) {
    LoadProject(name);
  }
  if (!current_project_) {
    CreateNewProject();
  }
  image_manager_->loadImages(monthes.size());
  return current_project_;
}

void FrameControl::SaveProject() {
  save_timer_->stop();

  FileSystemProjectWriter().Write(current_project_);

  save_timer_->start();
}

void FrameControl::SaveProjectMonth(int month) {
  save_timer_->stop();

  FileSystemProjectWriter().Write(current_project_, month);

  save_timer_->start();
}

void FrameControl::LoadProject(QString name) {
  current_project_ = FileSystemProjectLoader().Load(name);
}

ProjectPtr FrameControl::CurrentProject() { return current_project_; }

QString FrameControl::LastProjectName() const { return DEF_PROGECT_NAME; }

void FrameControl::CreateNewProject() {
  current_project_ = std::make_shared<Project>();
  current_project_->monthes_.resize(monthes.size());

  for (int month_number = 0; month_number < (int)monthes.size();
       month_number++) {
    auto &month = current_project_->monthes_[month_number];
    month.text = monthes[month_number];
    month.state |= (short)Core::MonthItem::STATE::CHANGED;
    month.photo_data = std::make_shared<PhotoData>();
    month.photo_data->resetData(QString::number(month_number), false);
  }

  SaveProject();
}

ImageManagerPtr FrameControl::imageManager() { return image_manager_; }

static const int c_image_scale_size = 1536;

void ImageManager::loadImages(int month_count) {

  if (!QDir().exists(project_images_path_)) {
    if (!QDir().mkpath(project_images_path_)) {
      spdlog::error("Error while creating directory {0}",
                    project_images_path_.toStdString());
      return;
    }
  } else {

    QDirIterator it(project_images_path_);

    while (it.hasNext()) {
      const auto fn = it.nextFileInfo();
      qDebug() << fn.fileName();

      if (it.fileInfo().isFile()) {
        fillImage(fn.absoluteFilePath(), fn.fileName());
      }
    }
  }
  for (int i = 0; i < (int)month_count; i++) {
    fillStubImage(QString::number(i));
  }
}

void ImageManager::fillImage(QString image_path, QString image_name) {
  QPixmap image(image_path);
  if (image.isNull()) {
    spdlog::info("Invalid image file {}", image_path.toStdString());
    return;
  }

  image.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());

  images_[image_name] = image;
}

void ImageManager::deleteImage(QString image_name) {
  UNI_ASSERT(!isStub(image_name));
  if (isStub(image_name)) {
    spdlog::error("Trying to delete stub {}", image_name.toStdString());
    return;
  }
  if (const auto it = images_.find(image_name); it != images_.end()) {
    images_.erase(it);
    QFile::remove(project_images_path_ + "/" + image_name);
  }
}

QString ImageManager::addImage(QString image_path) {
  QPixmap image(image_path);
  if (image.isNull()) {
    spdlog::error("Can't read image {}", image_path.toStdString());
    return QString();
  }
  QString new_image_name;
  QString new_image_path;

  if (image.width() > image.height()) {
    image = image.scaledToWidth(c_image_scale_size, Qt::SmoothTransformation);
  } else {
    image = image.scaledToHeight(c_image_scale_size, Qt::SmoothTransformation);
  }

  image.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());

  do {
    new_image_name = QString(c_photo_prefix_str) +
                     QString::number(QRandomGenerator::global()->generate());
    new_image_path = project_images_path_ + "/" + new_image_name;
  } while (QFileInfo::exists(new_image_path));

  image.save(new_image_path, IMAGE_FORMAT);
  images_[new_image_name] = image;

  spdlog::info("Image {} added", image_path.toStdString());

  return new_image_name;
}

void ImageManager::fillStubImage(QString month_name) {
  QString image_path(QString(c_stub_month_photo_template_str).arg(month_name));
  QPixmap image(image_path);

  image.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());

  images_[month_name] = image;
}
const QPixmap &ImageManager::image(QString image_name) const {
  if (const auto it = images_.find(image_name); it != images_.end())
    return it->second;
  else {
    spdlog::error("Image was nof found in cache {0}", image_name.toStdString());
    return empty_;
  }
}
bool ImageManager::isStub(QString image_name) const {
  return !image_name.startsWith(c_photo_prefix_str);
}

} // namespace FirstYear::Core
