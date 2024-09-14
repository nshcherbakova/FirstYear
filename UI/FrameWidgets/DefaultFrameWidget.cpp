#include "DefaultFrameWidget.h"
#include "PhotoTuneWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_open_image_str = "Open Image";
static const char *c_file_types_str = "Image Files (*.png *.jpg *.jpeg *.bmp)";
static const QStringList c_mime_type_filters({"image/jpeg", "image/pjpeg",
                                              "image/png", "image/bmp"});
static const char *c_stub_month_photo_template_str =
    ":images/frame/month_stub_%1";
static const char *c_last_opend_dir = "LAST_OPEND_DIRRECTORY_TO_LOAD_PHOTO";

DefaultFrameWidget::DefaultFrameWidget(QWidget &parent,
                                       Core::FrameControl &control)
    : QWidget(&parent), layout_(new QGridLayout()) {

  setContentsMargins(0, 0, 0, 0);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  // palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);

  photo_tune_widget_ = new PhotoTuneWidget(*this->parentWidget());
  photo_tune_widget_->hide();

  InitPhotos(control);

  spdlog::info("DefaultFrameWidget UI created");
}

QPixmap DefaultFrameWidget::GetStubPhoto(int month) {
  return QPixmap(QString(c_stub_month_photo_template_str).arg(month));
}

void DefaultFrameWidget::InitPhotos(Core::FrameControl &control) {

  auto project = control.CurrentProject();
  layout_->setGeometry(rect());
  layout_->setSizeConstraint(QLayout::SetFixedSize);
  photos_.resize(12);

  int row = 0;
  int column = 0;
  for (int i = 0; i < (int)photos_.size(); i++) {
    auto &month = project->monthes_[i];

    photos_[i] = new PhotoWidget(*this);
    auto &photo_widget = photos_[i];

    if (month.text)
      photo_widget->setText(*month.text);
    else
      photo_widget->setText(QString("%1 month").arg(i));

    if (month.photo_data.image.isNull()) {
      month.photo_data.is_stub_image = true;
      month.photo_data.image = GetStubPhoto(i);
      month.photo_data.angle = 0;
      month.photo_data.scale = 1;
      month.photo_data.offset = QPointF();
    }

    photo_widget->setPhoto(month.photo_data);
    photo_widget->show();
    layout_->addWidget(photo_widget, row, column);

    if (column == 3) {
      column = 0;
      row++;
    } else {
      column++;
    }
  }

  for (int i = 0; i < (int)photos_.size(); i++) {
    connect(photos_[i], &PhotoWidget::SignalImagePressed, this,
            [&, i, project] {
              auto &month = project->monthes_[i];
              if (month.photo_data.is_stub_image) {
                auto file = this->OpenFile();

                month.photo_data.image = QPixmap(file);
                month.photo_data.is_stub_image = false;
                month.photo_data.scale = 2.5;

                control.SaveProjectMonth(i);
                photos_[i]->setPhoto(month.photo_data);
              }

              photo_tune_widget_->setPhoto(i, photos_[i]->rect().size() * 2,
                                           month.photo_data);
              photo_tune_widget_->show();
            });

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalImageTuned, this,
            [&, i, project] {
              if (photo_tune_widget_->getPhotoId() == i) {
                const auto new_photo_data = photo_tune_widget_->getPhoto();
                photos_[i]->setPhoto(new_photo_data);
                project->monthes_[i].photo_data = new_photo_data;
                control.SaveProjectMonth(i);
              }
            });

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalPhotoChanged, this,
            [&, i, project] {
              if (photo_tune_widget_->getPhotoId() == i) {
                auto &month = project->monthes_[i];
                auto file = this->OpenFile();

                month.photo_data = {QPixmap(file), false, 0, 2.5, QPoint()};
                photo_tune_widget_->setPhoto(i, photos_[i]->rect().size() * 2,
                                             month.photo_data);
              }
            });
  }

  setLayout(layout_);
  update();
}

QString DefaultFrameWidget::OpenFile() {
  QString image_file_name;
  QString path;
  if (path.isEmpty()) {
    const QSettings settings(QSettings::Scope::UserScope);
    path = settings.value(c_last_opend_dir).toString();
  }

  QFileDialog dialog(this, c_open_image_str, path, c_file_types_str);

  dialog.setMimeTypeFilters(c_mime_type_filters);
  dialog.setFileMode(QFileDialog::ExistingFile);

  QStringList file_names;
  if (dialog.exec()) {
    file_names = dialog.selectedFiles();
  }

  if (!file_names.isEmpty()) {
    image_file_name = file_names.front();

    QSettings settings(QSettings::Scope::UserScope);
    settings.setValue(c_last_opend_dir,
                      QFileInfo(image_file_name).dir().path());
  }

  return image_file_name;
}
/*
void DefaultFrameWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  // Draw background
}*/

} // namespace FirstYear::UI
