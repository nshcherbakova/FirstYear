#include "Utility.h"
#include <stdafx.h>

namespace FirstYear::UI::Utility {

static const char *c_open_image_str = "Open Image";
static const char *c_file_types_str = "Image Files (*.png *.jpg *.jpeg *.bmp)";
static const QStringList c_mime_type_filters({"image/jpeg", "image/pjpeg",
                                              "image/png", "image/bmp"});
static const char *c_last_opend_dir = "LAST_OPEND_DIRRECTORY_TO_LOAD_PHOTO";

QString OpenFile(QWidget *parent) {

  QSettings settings(QSettings::Scope::UserScope);
  const QString path = settings.value(c_last_opend_dir).toString();

  const QString image_file_name = QFileDialog::getOpenFileName(
      parent, c_open_image_str, path, c_file_types_str);

  settings.setValue(c_last_opend_dir, QFileInfo(image_file_name).dir().path());

  return image_file_name;
}

QStringList OpenFiles(QWidget *parent) {

  QSettings settings(QSettings::Scope::UserScope);
  QString path = settings.value(c_last_opend_dir).toString();

  ImagePicker().showImagePicker();
  /* auto image_file_names = QFileDialog::getOpenFileNames(
       parent, c_open_image_str, path, c_file_types_str, nullptr,
       QFileDialog::ReadOnly);

   if (!image_file_names.empty()) {
     settings.setValue(c_last_opend_dir,
                       QFileInfo(image_file_names.front()).dir().path());
   }


   return image_file_names;*/
  return {};
}

} // namespace FirstYear::UI::Utility
