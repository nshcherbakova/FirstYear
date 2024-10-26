#include "DefaultTemplateWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_str = ":images/frame_%1/foreground";
static const char *c_foreground_to_render_str =
    ":images/frame_%1/foreground_to_render";
static const int c_max_title_lengh = 20;

class ForegroundWidget final : public QWidget {
public:
  ForegroundWidget(QWidget *parent, QPixmap image,
                   std::vector<QRectF> photo_slots)
      : QWidget(parent), photo_slots_(photo_slots), image_(image) {}

public:
  virtual void paintEvent(QPaintEvent *) override final {
    QPainter painter(this);

    painter.drawPixmap(rect(), image_to_paint_, rect());

    for (int i = 0; i < (int)photo_slots_.size(); i++) {
      auto new_rect = photo_slots_[i];
      painter.setPen(Qt::red);
      painter.drawRect(new_rect);
    }
  }

  virtual void resizeEvent(QResizeEvent *) override final {
    image_to_paint_ = image_.scaledToWidth(width(), Qt::SmoothTransformation);
  }

private:
  std::vector<QRectF> photo_slots_;
  QPixmap image_;
  QPixmap image_to_paint_;
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

DefaultTemplateWidget::DefaultTemplateWidget(QWidget *parent,
                                             Core::FrameControl &control)

    : TemplateWidgetBase(parent,
                         {control,
                          "1",
                          {{40, 5, 600, 15}, Qt::AlignCenter},
                          {{{35, 160},
                            {184, 160},
                            {328, 160},
                            {476, 160},
                            {35, 310},
                            {184, 310},
                            {328, 310},
                            {476, 310},
                            {35, 455},
                            {184, 455},
                            {328, 455},
                            {476, 455}},
                           Qt::AlignLeft},
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
                          {{FrameParameters::TYPE::ROUND, QSizeF{125, 125}}}}) {

  // reload(control);
}

DefaultTemplateWidget2::DefaultTemplateWidget2(QWidget *parent,
                                               Core::FrameControl &control)

    : TemplateWidgetBase(parent,
                         {control,
                          "2",
                          {{40, 5, 600, 15}, Qt::AlignCenter},
                          {{{97, 160},
                            {246, 160},
                            {390, 160},
                            {538, 160},
                            {97, 310},
                            {246, 310},
                            {390, 310},
                            {538, 310},
                            {97, 455},
                            {246, 455},
                            {390, 455},
                            {538, 455}},
                           Qt::AlignCenter},

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
                           {FrameParameters::TYPE::ROUND, QSizeF{125, 125}}}}) {

  // reload(control);
}

TemplateWidgetBase::TemplateWidgetBase(
    QWidget *parent, const TemplateWidgetParameters &parameters)
    : QWidget(parent), id_(parameters.id),
      foreground_(QString(c_foreground_str).arg(parameters.id)),
      foreground_to_render_(
          QString(c_foreground_to_render_str).arg(parameters.id)),
      title_slot_real_(std::move(parameters.title_parameters.title_rect)),
      photo_slots_real_(std::move(parameters.photo_slots)),
      photo_text_anchors_real_(
          std::move(parameters.photo_text_parameters.photo_text_anchors)),
      // photo_text_aligment_(parameters.photo_text_parameters.aligment),
      frame_data_(std::move(parameters.frame_data)),
      control_(parameters.control) {

  UNI_ASSERT(frame_data_.size() == 1 || frame_data_.size() == 12);
  UNI_ASSERT(photo_slots_real_.size() == 12);
  UNI_ASSERT(photo_text_anchors_real_.size() == 12);
  UNI_ASSERT(!id_.isEmpty());

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

  initMonthPhotoWidgets(control_);

  createButtons(control_);
  createForegroundWidget();

  text_widget_ = new QLineEdit(this);
  text_widget_->raise();
  text_widget_->setAlignment(parameters.title_parameters.aligment);
  text_widget_->setMaxLength(c_max_title_lengh);
  connect(text_widget_, &QLineEdit::textEdited, this, [&] {
    auto project = control_.CurrentProject();
    project->title_ = text_widget_->text();
    control_.SaveProject();
    emit SignalTextChanged();
  });

  photo_text_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_text_widgets_[i] = new QLabel(this);
    photo_text_widgets_[i]->setAlignment(
        parameters.photo_text_parameters.aligment);

    connect(photo_text_widgets_[i], &QLabel::linkActivated, this, [&] {
      //  auto project = control_.CurrentProject();
      // project->title_ = text_widget_->text();
      control_.SaveProject();
      emit SignalTextChanged();
    });
  }
}

QString TemplateWidgetBase::id() const { return id_; }

void TemplateWidgetBase::initMonthPhotoWidgets(Core::FrameControl &control) {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

    connect(photo_widgets_[i], &PhotoWidget::SignalImagePressed, this, [&, i] {
      auto project = control.CurrentProject();
      auto &month = project->monthes_[i];

      if (month.photo_data.is_stub_image) {
        const auto file = Utility::OpenFile(this);
        if (!file.isNull()) {
          month.photo_data.is_stub_image = false;
          month.photo_data.image = QPixmap(file);
          //  month.photo_data.scale = 2.5;

          control.SaveProjectMonth(i);
          photo_widgets_[i]->setPhoto(month.photo_data);
        }
      }

      if (!month.photo_data.is_stub_image) {
        emit SignalTunePhoto(i, frame_data_[i], month.photo_data);
      }
    });
  }
}

void TemplateWidgetBase::createForegroundWidget() {
  foreground_widget_ = new ForegroundWidget(this, foreground_, photo_slots_);
  foreground_widget_->setGeometry(geometry());
  foreground_widget_->raise();
  foreground_widget_->show();
  foreground_widget_->setContentsMargins(0, 0, 0, 0);
  foreground_widget_->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TemplateWidgetBase::createButtons(Core::FrameControl &control) {

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

void TemplateWidgetBase::Update() {
  QRect rect = {QPoint(0, 0), QWidget::size()};
  foreground_widget_->setGeometry(rect);
  render_button_->setGeometry(20, rect.height() - 2 * 40, 2 * 40, 40);
  share_button_->setGeometry(rect.width() - 100, rect.height() - 2 * 40, 2 * 40,
                             40);
  load(control_);

  text_widget_->setGeometry(title_slot_real_.left(), title_slot_real_.top(),
                            title_slot_real_.width(),
                            title_slot_real_.height());

  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {

    auto &photo_text_widget = photo_text_widgets_[i];

    QRect rect;
    if (photo_text_widget->alignment() == Qt::AlignCenter) {
      rect =
          QRect{photo_text_anchors_[i].x() - photo_text_widget->width() / 2,
                photo_text_anchors_[i].y(), photo_text_widget->width(),
                photo_text_widget->heightForWidth(photo_text_widget->width())};
    } else if (photo_text_widget->alignment() == Qt::AlignLeft) {
      rect =
          QRect{photo_text_anchors_[i].x(), photo_text_anchors_[i].y(),
                photo_text_widget->width(),
                photo_text_widget->heightForWidth(photo_text_widget->width())};
    }
    photo_text_widget->setGeometry(rect);
  }
}

void TemplateWidgetBase::resizeEvent(QResizeEvent *) { Update(); }

void TemplateWidgetBase::load(Core::FrameControl &control) {

  double k = (double)width() / foreground_.width();

  // adjust coordinatrs
  for (int i = 0; i < (int)photo_slots_real_.size(); i++) {
    auto new_rect = photo_slots_real_[i];
    new_rect.setTopLeft(photo_slots_real_[i].topLeft() * k);
    new_rect.setSize(photo_slots_real_[i].size() * k);

    photo_slots_[i] = new_rect;
  }

  photo_text_anchors_.resize(photo_text_anchors_real_.size());
  for (int i = 0; i < (int)photo_text_anchors_real_.size(); i++) {
    auto new_point = photo_text_anchors_real_[i] * k;
    photo_text_anchors_[i] = new_point;
  }

  auto new_rect = title_slot_real_;
  new_rect.setTopLeft(title_slot_real_.topLeft() * k);
  new_rect.setSize(title_slot_real_.size() * k);
  title_slot_ = new_rect;

  InitPhotos(control);

  auto project = control.CurrentProject();
  text_widget_->setText(project->title_);
}

void TemplateWidgetBase::InitPhotos(Core::FrameControl &control) {

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_widget = photo_widgets_[i];

    photo_widget->setGeometry(photo_slots_[i].toRect());

    /* if (month.text)
       photo_widget->setText(*month.text);
     else
       photo_widget->setText(QString("%1 month").arg(i));*/

    photo_widget->setPhoto(month.photo_data);
    photo_widget->show();
  }

  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {
    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_text_widget = photo_text_widgets_[i];

    if (month.text)
      photo_text_widget->setText(*month.text);
    else
      photo_text_widget->setText(QString("%1 month").arg(i));
  }

  update();
}

void TemplateWidgetBase::setVisible(bool visible) {
  if (visible) {
    load(control_);
  }
  QWidget::setVisible(visible);
}
void TemplateWidgetBase::paintEvent(QPaintEvent *e) {
  QWidget::paintEvent(e);
  QPainter painter(this);
  for (auto rect : photo_slots_) {
    painter.setBrush(Qt::red);
    painter.drawRect(rect);
  }

  // Draw background
}

QPixmap TemplateWidgetBase::renderFrame(FirstYear::Core::ProjectPtr project) {
  QPixmap pixmap(foreground_to_render_.size() * devicePixelRatio());
  pixmap.setDevicePixelRatio(devicePixelRatio());
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
