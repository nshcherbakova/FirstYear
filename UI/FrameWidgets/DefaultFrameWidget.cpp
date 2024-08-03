#include "DefaultFrameWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_open_image_str = "Open Image";
static const char *c_file_types_str = "Image Files (*.png *.jpg *.jpeg *.bmp)";
static const QStringList c_mime_type_filters({"image/jpeg", "image/pjpeg",
                                              "image/png", "image/bmp"});

DefaultFrameWidget::DefaultFrameWidget(QWidget &parent)
    : QWidget(&parent), layout_(new QGridLayout()) {

  setContentsMargins(0, 0, 0, 0);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  // palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);

  InitPhotos();

  spdlog::info("DefaultFrameWidget UI created");
}

void DefaultFrameWidget::InitPhotos() {

  layout_->setSizeConstraint(QLayout::SetFixedSize);
  photos_.resize(12);

  int row = 0;
  int column = 0;
  for (size_t i = 0; i < photos_.size(); i++) {
    auto &photo = photos_[i];
    photo = new PhotoWidget(*this);
    photo->setText(QString("%1 month").arg(i));
    photo->setImage(QPixmap(":images/frame/month_stub"));
    connect(photo, &PhotoWidget::SignalImagePressed, this, [&] {
      auto file = this->OpenFile();
      QPixmap picture(file);
      photo->setImage(picture);
    });
    photo->show();

    layout_->addWidget(photo, row, column);
    if (column == 3) {
      column = 0;
      row++;
    } else {
      column++;
    }
  }

  //  layout_->setGeometry(geometry());
  //   layout_->addWidget(new QLabel(":images/frame/month_stub"));
  setLayout(layout_);
}

QString DefaultFrameWidget::OpenFile() {
  QString image_file_name;
  QString path = image_file_name;
  /*  if (path.isEmpty()) {
        const QSettings settings(QSettings::Scope::UserScope);
        path = settings.value(c_last_opend_dir).toString();
    }*/

  QFileDialog dialog(this, c_open_image_str, path, c_file_types_str);

  dialog.setMimeTypeFilters(c_mime_type_filters);
  dialog.setFileMode(QFileDialog::ExistingFile);

  QStringList file_names;
  if (dialog.exec()) {
    file_names = dialog.selectedFiles();
  }

  if (!file_names.isEmpty()) {
    image_file_name = file_names.front();

    ///   QSettings settings(QSettings::Scope::UserScope);
    //   settings.setValue(c_last_opend_dir,
    //                     QFileInfo(image_file_name_).dir().path());
  }

  return image_file_name;
}
/*
void DefaultFrameWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
}*/

} // namespace FirstYear::UI
