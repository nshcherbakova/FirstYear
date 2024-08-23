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

  InitPhotos(control);

  photo_tune_widget_ = new PhotoTuneWidget(*this->parentWidget());
  photo_tune_widget_->hide();
  //  photo_tune_widget_->setGeometry(parent.geometry());
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
  for (size_t i = 0; i < photos_.size(); i++) {
    auto &photo_widget = photos_[i];
    photo_widget = new PhotoWidget(*this);

    if (project->monthes_[i].text)
      photo_widget->setText(*project->monthes_[i].text);
    else
      photo_widget->setText(QString("%1 month").arg(i));

    QPixmap photo;
    if (project->monthes_[i].photo)
      photo = *project->monthes_[i].photo;
    else
      photo = GetStubPhoto(i);

    QRectF photo_rect = photo.rect();
    QRectF widget_rect = photo_widget->rect();
    double k1 = photo_rect.width() / photo_rect.height();
    double k2 = widget_rect.width() / widget_rect.height();
    double scale = 1;
    QPoint offset;
    if (k1 > k2) {
      scale = photo_rect.height() / widget_rect.height();
      offset.setX(photo_rect.width() - widget_rect.width() * scale);
    } else {
      scale = photo_rect.width() / widget_rect.width();
      offset.setY(photo_rect.height() - widget_rect.height() * scale);
    }
    photo_widget->setPhoto({photo, scale, 0, offset});

    connect(photo_widget, &PhotoWidget::SignalImagePressed, this,
            [&, i, project, photo] {
              /*     auto file = this->OpenFile();
                   QPixmap picture(file);
                   photo_widget->setPhoto({picture, scale, offset});
                   project->monthes_[i].photo = picture;
                   control.SaveProjectMonth(i);*/

              //   photo_tune_widget_->setGeometry(this->geometry());
              photo_tune_widget_->setPhoto({photo, scale, 0, offset});
              photo_tune_widget_->show();
            });
    photo_widget->show();

    layout_->addWidget(photo_widget, row, column);

    if (column == 3) {
      column = 0;
      row++;
    } else {
      column++;
    }
  }

  setLayout(layout_);
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
