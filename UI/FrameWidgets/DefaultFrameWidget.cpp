#include "DefaultFrameWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_str = ":images/frame_%1/foreground";
static const char *c_foreground_to_render_str =
    ":images/frame_%1/foreground_to_render";

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

DefaultFrameWidget::DefaultFrameWidget(QWidget *parent,
                                       Core::FrameControl &control)

    : FrameWidgetBase(parent, control, "1",
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

DefaultFrameWidget2::DefaultFrameWidget2(QWidget *parent,
                                         Core::FrameControl &control)

    : FrameWidgetBase(parent, control, "2",
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

FrameWidgetBase::FrameWidgetBase(QWidget *parent, Core::FrameControl &control,
                                 QString id, std::vector<QRectF> photo_slots,
                                 std::vector<FrameParameters> frame_data)
    : QWidget(parent), id_(id), foreground_(QString(c_foreground_str).arg(id_)),
      foreground_to_render_(QString(c_foreground_to_render_str).arg(id_)),
      photo_slots_real_(std::move(photo_slots)),
      frame_data_(std::move(frame_data)), control_(control) {

  UNI_ASSERT(frame_data_.size() == 1 || frame_data_.size() == 12);
  UNI_ASSERT(photo_slots_real_.size() == 12);
  UNI_ASSERT(!id.isEmpty());

  setContentsMargins(0, 0, 0, 0);
  setAutoFillBackground(true);

  if (frame_data_.size() == 1) {
    frame_data_.resize(12, frame_data_[0]);
  }

  photo_slots_.resize(12);

  photo_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] = new PhotoWidget(*this);
  }

  initMonthPhotoWidgets(control);

  createButtons(control);
  createForegroundWidget();
}

QString FrameWidgetBase::id() const { return id_; }

void FrameWidgetBase::initMonthPhotoWidgets(Core::FrameControl &control) {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

    connect(photo_widgets_[i], &PhotoWidget::SignalImagePressed, this, [&, i] {
      auto project = control.CurrentProject();
      auto &month = project->monthes_[i];

      if (month.photo_data.is_stub_image) {
        auto file = Utility::OpenFile(this);

        month.photo_data.image = QPixmap(file);
        month.photo_data.is_stub_image = false;
        month.photo_data.scale = 2.5;

        control.SaveProjectMonth(i);
        photo_widgets_[i]->setPhoto(month.photo_data);
      }
      emit SignalTunePhoto(i, frame_data_[i], month.photo_data);
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

  render_button_ = new QPushButton(this);
  render_button_->setGeometry(20, geometry().height() - 2 * 40, 2 * 40, 40);
  render_button_->setText("Render");
  render_button_->setContentsMargins(0, 0, 0, 0);
  connect(render_button_, &QPushButton::clicked, this, [&] {
    auto pixmap = this->renderFrame(control.CurrentProject());
    pixmap.save("/Users/nshcherbakova/Desktop/FirstYear/test1.png");
    QLabel *l = new QLabel("test", this);
    l->setPixmap(pixmap);
    l->show();
  });

  share_button_ = new QPushButton(this);
  share_button_->setGeometry(geometry().width() - 100,
                             geometry().height() - 2 * 40, 2 * 40, 40);
  share_button_->setText("Share");
  share_button_->setContentsMargins(0, 0, 0, 0);
  connect(share_button_, &QPushButton::clicked, this, [&] {

  });
}

void FrameWidgetBase::Update() {
  QRect rect = {QPoint(0, 0), QWidget::size()};
  foreground_widget_->setGeometry(rect);
  render_button_->setGeometry(20, rect.height() - 2 * 40, 2 * 40, 40);
  share_button_->setGeometry(rect.width() - 100, rect.height() - 2 * 40, 2 * 40,
                             40);
  load(control_);
}

void FrameWidgetBase::resizeEvent(QResizeEvent *) { Update(); }

void FrameWidgetBase::load(Core::FrameControl &control) {

  double k = (double)geometry().width() / foreground_.width();

  for (int i = 0; i < (int)photo_slots_real_.size(); i++) {
    auto new_rect = photo_slots_real_[i];
    new_rect.setTopLeft(photo_slots_real_[i].topLeft() * k);
    new_rect.setSize(photo_slots_real_[i].size() * k);

    photo_slots_[i] = new_rect;
  }

  InitPhotos(control);
}

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
