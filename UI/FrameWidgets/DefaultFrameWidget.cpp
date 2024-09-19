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
    ":images/frame_%1/month_stub_%2";
static const char *c_last_opend_dir = "LAST_OPEND_DIRRECTORY_TO_LOAD_PHOTO";
static const char *c_foreground_str = ":images/frame_%1/foreground";

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags)
    : QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent *) { emit clicked(); }

class ForegroundWidget final : public QWidget {
public:
  ForegroundWidget(QWidget *parent, QPixmap image)
      : QWidget(parent), image_(image) {
    update();
  }

public:
  virtual void paintEvent(QPaintEvent *) override final {
    QPainter painter(this);
    image_ = image_.scaledToWidth(width());
    painter.drawPixmap(rect(), image_, rect());
  }

private:
  QPixmap image_;
};
DefaultFrameWidget::DefaultFrameWidget(QWidget &parent,
                                       Core::FrameControl &control)
    : QWidget(&parent) /*, layout_(new QGridLayout())*/, id_("1"),
      foreground_(QString(c_foreground_str).arg(id_)),
      photo_slots_({{35, 35, 125, 125},
                    {184, 35, 125, 125},
                    {328, 35, 125, 125},
                    {476, 35, 125, 125},
                    {35, 185, 125, 125},
                    {184, 185, 125, 125},
                    {328, 185, 125, 125},
                    {476, 185, 125, 125},
                    {35, 330, 125, 125},
                    {184, 330, 125, 125},
                    {328, 330, 125, 125},
                    {476, 330, 125, 125}}) {

  setContentsMargins(0, 0, 0, 0);
  setGeometry(parent.geometry());
  setAutoFillBackground(true);
  auto palette = QWidget::palette();
  // palette.setColor(QPalette::Window, Qt::red);
  setPalette(palette);

  photo_tune_widget_ = new PhotoTuneWidget(*this->parentWidget());
  photo_tune_widget_->hide();

  InitPhotos(control);

  foreground_widget_ = new ForegroundWidget(this, foreground_);
  foreground_widget_->setGeometry(geometry());
  foreground_widget_->raise();
  foreground_widget_->show();
  foreground_widget_->setAttribute(Qt::WA_TransparentForMouseEvents);
  spdlog::info("DefaultFrameWidget UI created");
}

QPixmap DefaultFrameWidget::GetStubPhoto(int month) {
  return QPixmap(QString(c_stub_month_photo_template_str).arg(id_).arg(month));
}

void DefaultFrameWidget::InitPhotos(Core::FrameControl &control) {

  auto project = control.CurrentProject();
  //  layout_->setGeometry(rect());
  //  layout_->setSizeConstraint(QLayout::SetFixedSize);
  photo_widgets_.resize(12);

  for (int i = 0; i < (int)project->monthes_.size(); i++) {
    auto &month = project->monthes_[i];

    if (month.photo_data.image.isNull()) {
      month.photo_data.is_stub_image = true;
      month.photo_data.image = GetStubPhoto(i);
      month.photo_data.angle = 0;
      month.photo_data.scale = 1;
      month.photo_data.offset = QPoint();
    }
  }

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    auto &month = project->monthes_[i];

    photo_widgets_[i] = new PhotoWidget(*this);
    auto &photo_widget = photo_widgets_[i];
    photo_widget->setGeometry(photo_slots_[i]);

    if (month.text)
      photo_widget->setText(*month.text);
    else
      photo_widget->setText(QString("%1 month").arg(i));

    photo_widget->setPhoto(month.photo_data, photo_slots_[i]);
    photo_widget->show();
  }

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    connect(photo_widgets_[i], &PhotoWidget::SignalImagePressed, this,
            [&, i, project] {
              auto &month = project->monthes_[i];
              if (month.photo_data.is_stub_image) {
                auto file = this->OpenFile();

                month.photo_data.image = QPixmap(file);
                month.photo_data.is_stub_image = false;
                month.photo_data.scale = 2.5;

                control.SaveProjectMonth(i);
                photo_widgets_[i]->setPhoto(month.photo_data, photo_slots_[i]);
              }

              photo_tune_widget_->setPhoto(i, photo_slots_[i].size() * 2,
                                           month.photo_data);
              photo_tune_widget_->show();
            });

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalImageTuned, this,
            [&, i, project] {
              if (photo_tune_widget_->getPhotoId() == i) {
                const auto new_photo_data = photo_tune_widget_->getPhoto();
                photo_widgets_[i]->setPhoto(new_photo_data, photo_slots_[i]);
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
                photo_tune_widget_->setPhoto(i, photo_slots_[i].size() * 2,
                                             month.photo_data);
              }
            });
  }

  myLabel_ = new ClickableLabel(this->parentWidget());
  myLabel_->setGeometry(geometry());
  myLabel_->hide();
  connect(myLabel_, &ClickableLabel::clicked, this, [&] { myLabel_->hide(); });

  auto open_file = new QPushButton(this);
  open_file->setGeometry(20, geometry().height() - 2 * 40, 2 * 40, 40);
  open_file->setText("Render");
  open_file->setContentsMargins(0, 0, 0, 0);
  connect(open_file, &QPushButton::clicked, this, [&, project] {
    QPixmap pixmap(this->sizeHint());
    QPainter painter(&pixmap);
    this->render(&painter);
    // pixmap.save("/Users/nshcherbakova/Desktop/FirstYear/test.png");

    myLabel_->setPixmap(pixmap);

    myLabel_->show();
  });

  // layout_->addWidget(open_file, 4, 0);
  // setLayout(layout_);
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

void DefaultFrameWidget::paintEvent(QPaintEvent *e) {
  QWidget::paintEvent(e);
  QPainter painter(this);
  for (auto rect : photo_slots_) {
    painter.setBrush(Qt::red);
    painter.drawRect(rect);
  }

  // Draw background
}

} // namespace FirstYear::UI
