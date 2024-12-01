#include "Utility.h"
#include <stdafx.h>

#ifndef Q_OS_ANDROID
static const char *c_open_image_str = "Open Image";
static const char *c_file_types_str = "Image Files (*.png *.jpg *.jpeg *.bmp)";
static const QStringList c_mime_type_filters({"image/jpeg", "image/pjpeg",
                                              "image/png", "image/bmp"});
#endif

namespace FirstYear::UI::Utility {

OpenFileDialog::OpenFileDialog(QObject *parent) : QObject(parent) {
#ifdef Q_OS_ANDROID
  connect(&single_image_picker_,
          &FirstYear::Core::Android::SingleImagePicker::SignalPickedImage, this,
          &FirstYear::UI::Utility::OpenFileDialog::SignalPickedImage);
  connect(&images_picker_,
          &FirstYear::Core::Android::ImagesPicker::SignalPickedImages, this,
          &FirstYear::UI::Utility::OpenFileDialog::SignalPickedImages);
#endif
}

void OpenFileDialog::OpenFile() {
#ifdef Q_OS_ANDROID
  single_image_picker_.show();
#else
  const QString image_file_name = QFileDialog::getOpenFileName(
      nullptr, c_open_image_str, QString(), c_file_types_str);
  emit SignalPickedImage(image_file_name);
#endif
}

void OpenFileDialog::OpenFiles() {
#ifdef Q_OS_ANDROID
  images_picker_.show();
#else
  auto image_file_names = QFileDialog::getOpenFileNames(
      nullptr, c_open_image_str, QString(), c_file_types_str, nullptr,
      QFileDialog::ReadOnly);
  emit SignalPickedImages(image_file_names);
#endif
}

} // namespace FirstYear::UI::Utility
