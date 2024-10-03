#include "DefaultFrameWidget.h"
#include "PhotoTuneWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_open_image_str = "Open Image";
static const char *c_file_types_str = "Image Files (*.png *.jpg *.jpeg *.bmp)";
static const QStringList c_mime_type_filters({"image/jpeg", "image/pjpeg",
                                              "image/png", "image/bmp"});

static const char *c_last_opend_dir = "LAST_OPEND_DIRRECTORY_TO_LOAD_PHOTO";

static const char *c_foreground_str = ":images/frame_%1/foreground";
static const char *c_foreground_to_render_str =
    ":images/frame_%1/foreground_to_render";

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags)
    : QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent *) { emit clicked(); }

class ForegroundWidget final : public QWidget {
public:
  ForegroundWidget(QWidget *parent, QPixmap image,
                   std::vector<QRectF> photo_slots)
      : QWidget(parent), photo_slots_(photo_slots), image_(image) {}

public:
  virtual void paintEvent(QPaintEvent *) override final {
    QPainter painter(this);
    image_ = image_.scaledToWidth(width());

    painter.drawPixmap(rect(), image_, rect());

    for (int i = 0; i < (int)photo_slots_.size(); i++) {
      auto new_rect = photo_slots_[i];
      painter.setPen(Qt::red);
      painter.drawRect(new_rect);
    }
  }

private:
  std::vector<QRectF> photo_slots_;
  QPixmap image_;
};

class ForegroundWidget2 final : public QWidget {
public:
  ForegroundWidget2(QWidget *parent) : QWidget(parent) {
    static const char *c_image_widget_button_style_str =
        "ForegroundWidget2{ background-color: rgba(255, 255, 255, 0);}";
    setStyleSheet(c_image_widget_button_style_str);
  }

public:
private:
};

DefaultFrameWidget::DefaultFrameWidget(QWidget &parent,
                                       QWidget *tune_widget_parent,
                                       Core::FrameControl &control)

    : FrameWidgetBase(parent, tune_widget_parent, control, "1",
                      {{35, 35, 125, 125},
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
                       {476, 330, 125, 125}},
                      {{FrameParameters::TYPE::ROUND, QSizeF{125, 125}}}) {

  // reload(control);
}

DefaultFrameWidget2::DefaultFrameWidget2(QWidget &parent,
                                         QWidget *tune_widget_parent,
                                         Core::FrameControl &control)

    : FrameWidgetBase(parent, tune_widget_parent, control, "2",
                      {{35, 35, 125, 125},
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
                       {476, 330, 125, 125}},

                      {{FrameParameters::TYPE::RECT, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::RECT, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}},
                       {FrameParameters::TYPE::ROUND, QSizeF{125, 125}}}) {

  // reload(control);
}

FrameWidgetBase::FrameWidgetBase(QWidget &parent, QWidget *tune_widget_parent,
                                 Core::FrameControl &control, QString id,
                                 std::vector<QRectF> photo_slots,
                                 std::vector<FrameParameters> frame_data)
    : QWidget(&parent), id_(id),
      foreground_(QString(c_foreground_str).arg(id_)),
      foreground_to_render_(QString(c_foreground_to_render_str).arg(id_)),
      photo_slots_real_(std::move(photo_slots)),
      frame_data_(std::move(frame_data)), control_(control) {

  UNI_ASSERT(frame_data_.size() == 1 || frame_data_.size() == 12);
  UNI_ASSERT(photo_slots_real_.size() == 12);
  UNI_ASSERT(!id.isEmpty());

  setContentsMargins(0, 0, 0, 0);
  setGeometry(tune_widget_parent->geometry());
  setAutoFillBackground(true);

  if (frame_data_.size() == 1) {
    frame_data_.resize(12, frame_data_[0]);
  }

  photo_slots_.resize(12);
  photo_tune_widget_ = new PhotoTuneWidget(*tune_widget_parent);
  photo_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] = new PhotoWidget(*this);
  }

  initPhotoTuneWidget(control);
  initMonthPhotoWidgets(control);

  createButtons(control);
  createForegroundWidget();

  /* ForegroundWidget2* foreground_widget= new ForegroundWidget2(this);
   foreground_widget->setGeometry(geometry());
   foreground_widget->raise();
   foreground_widget->show();*/
  // foreground_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
}

QString FrameWidgetBase::id() const { return id_; }

void FrameWidgetBase::initPhotoTuneWidget(Core::FrameControl &control) {

  photo_tune_widget_->hide();

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalImageTuned, this,
            [&, i] {
              auto project = control.CurrentProject();

              if (photo_tune_widget_->getPhotoId() == i) {
                const auto new_photo_data = photo_tune_widget_->getPhoto();

                // photo_widgets_[i]->setPhoto(new_photo_data);
                project->monthes_[i].photo_data = new_photo_data;

                // OnSignalUpdate();
                control.SaveProjectMonth(i);
              }
            });

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalPhotoChanged, this,
            [&, i] {
              auto project = control.CurrentProject();

              if (photo_tune_widget_->getPhotoId() == i) {
                auto &month = project->monthes_[i];
                auto file = this->OpenFile();

                month.photo_data = {QPixmap(file), false, 0, 2.5, QPoint()};

                // OnSignalUpdate();
                // photo_tune_widget_->setPhoto(i, frame_data_[i],
                // month.photo_data);
              }
            });

    connect(photo_tune_widget_, &PhotoTuneWidget::SignalImageTuned, this,
            &FrameWidgetBase::SignalUpdate);
    connect(photo_tune_widget_, &PhotoTuneWidget::SignalPhotoChanged, this,
            &FrameWidgetBase::SignalUpdate);
  }
}

void FrameWidgetBase::initMonthPhotoWidgets(Core::FrameControl &control) {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

    connect(photo_widgets_[i], &PhotoWidget::SignalImagePressed, this, [&, i] {
      auto project = control.CurrentProject();
      auto &month = project->monthes_[i];

      if (month.photo_data.is_stub_image) {
        auto file = this->OpenFile();

        month.photo_data.image = QPixmap(file);
        month.photo_data.is_stub_image = false;
        month.photo_data.scale = 2.5;

        control.SaveProjectMonth(i);
        photo_widgets_[i]->setPhoto(month.photo_data);
      }

      photo_tune_widget_->setPhoto(i, frame_data_[i], month.photo_data);
      photo_tune_widget_->show();
    });
  }
}

void FrameWidgetBase::createForegroundWidget() {
  foreground_widget_ = new ForegroundWidget(this, foreground_, photo_slots_);
  foreground_widget_->setGeometry(geometry());
  foreground_widget_->raise();
  foreground_widget_->show();
  foreground_widget_->setContentsMargins(0, 0, 0, 0);
  foreground_widget_->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void FrameWidgetBase::createButtons(Core::FrameControl &control) {

  myLabel_ = new ClickableLabel(this->parentWidget());
  myLabel_->setGeometry(geometry());
  myLabel_->hide();
  connect(myLabel_, &ClickableLabel::clicked, this, [&] { myLabel_->hide(); });

  auto render = new QPushButton(this);
  render->setGeometry(20, geometry().height() - 2 * 40, 2 * 40, 40);
  render->setText("Render");
  render->setContentsMargins(0, 0, 0, 0);
  connect(render, &QPushButton::clicked, this, [&] {
    auto pixmap = this->renderFrame(control.CurrentProject());
    pixmap.save("/Users/nshcherbakova/Desktop/FirstYear/test1.png");

    myLabel_->setPixmap(pixmap);
    myLabel_->show();
  });

  auto share = new QPushButton(this);
  share->setGeometry(geometry().width() - 100, geometry().height() - 2 * 40,
                     2 * 40, 40);
  share->setText("Share");
  share->setContentsMargins(0, 0, 0, 0);
  connect(share, &QPushButton::clicked, this, [&] {

  });

  /*  auto back = new QPushButton(this);
    back->setGeometry(20, geometry().height() - 4 * 40, 2 * 40, 40);
    back->setText("<");
    back->setContentsMargins(0, 0, 0, 0);
    connect(back, &QPushButton::clicked, this, [&] { control.previousFrame();
    });

    auto next = new QPushButton(this);
    next->setGeometry(geometry().width() - 100, geometry().height() - 4 * 40,
                      2 * 40, 40);
    next->setText(">");
    next->setContentsMargins(0, 0, 0, 0);
    connect(next, &QPushButton::clicked, this, [&] { control.nextFrame(); });*/
}

void FrameWidgetBase::Update() { load(control_); }

void FrameWidgetBase::load(Core::FrameControl &control) {

  double k = (double)geometry().width() / foreground_.width();

  for (int i = 0; i < (int)photo_slots_real_.size(); i++) {
    auto new_rect = photo_slots_real_[i];
    new_rect.setTopLeft(photo_slots_real_[i].topLeft() * k);
    new_rect.setSize(photo_slots_real_[i].size() * k);

    photo_slots_[i] = new_rect;
  }

  InitPhotos(control);

  // QWidget::show();
}

// void FrameWidgetBase::hide() { QWidget::hide(); }

void FrameWidgetBase::InitPhotos(Core::FrameControl &control) {

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_widget = photo_widgets_[i];

    photo_widget->setGeometry(photo_slots_[i].toRect());

    if (month.text)
      photo_widget->setText(*month.text);
    else
      photo_widget->setText(QString("%1 month").arg(i));

    photo_widget->setPhoto(month.photo_data);
    photo_widget->show();
  }

  update();
}

QString FrameWidgetBase::OpenFile() {
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
void FrameWidgetBase::setVisible(bool visible) {
  if (visible) {
    load(control_);
  }
  QWidget::setVisible(visible);
}
void FrameWidgetBase::paintEvent(QPaintEvent *e) {
  QWidget::paintEvent(e);
  QPainter painter(this);
  for (auto rect : photo_slots_) {
    painter.setBrush(Qt::red);
    painter.drawRect(rect);
  }

  // Draw background
}

QPixmap FrameWidgetBase::renderFrame(FirstYear::Core::ProjectPtr project) {
  QPixmap pixmap(foreground_to_render_.size());
  //

  std::vector<QRectF> photo_slots_for_render;
  photo_slots_for_render.resize(photo_slots_real_.size());
  double k = (double)foreground_to_render_.width() / foreground_.width();
  for (int i = 0; i < (int)photo_slots_real_.size(); i++) {
    auto new_rect = photo_slots_real_[i];
    new_rect.setTopLeft(photo_slots_real_[i].topLeft() * k);
    new_rect.setSize(photo_slots_real_[i].size() * k);
    photo_slots_for_render[i] = new_rect;
  }

  for (int i = 0; i < (int)project->monthes_.size(); i++) {
    QPainter painter_for_photo(&pixmap);
    QRectF slot_rect = photo_slots_for_render[i];
    painter_for_photo.setClipRect(slot_rect);

    PhotoPainter photo_painter;
    photo_painter.init(project->monthes_[i].photo_data, slot_rect, slot_rect);
    photo_painter.drawPhoto(painter_for_photo);
  }

  QPainter painter(&pixmap);
  painter.drawPixmap(pixmap.rect(), foreground_to_render_,
                     foreground_to_render_.rect());
  return pixmap;
  // Draw background
}

} // namespace FirstYear::UI
