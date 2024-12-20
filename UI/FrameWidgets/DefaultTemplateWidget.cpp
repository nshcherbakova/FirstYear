#include "DefaultTemplateWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_str = ":images/frame_%1/foreground";
static const char *c_foreground_to_render_str =
    ":images/frame_%1/foreground_to_render";
// static const int c_max_title_lengh = 20;

class ForegroundWidget final : public QWidget {
public:
  ForegroundWidget(QWidget *parent, QPixmap image)
      : QWidget(parent), image_(image) {}

public:
  virtual void paintEvent(QPaintEvent *) override final {
    QPainter painter(this);

    painter.drawPixmap(rect(), image_to_paint_, rect());
  }

  virtual void resizeEvent(QResizeEvent *e) override final {
    QWidget::resizeEvent(e);
    image_to_paint_ = image_.scaledToWidth(width(), Qt::SmoothTransformation);
  }

  QSize preferedSize(QSize size) const {
    QSize result;
    float widget_k = (float)image_.width() / (float)image_.height();
    float size_k = (float)size.width() / (float)size.height();
    if (widget_k < size_k) {
      result.setHeight(size.height());
      result.setWidth(image_.width() * (float)size.height() /
                      (float)image_.height());
    } else {
      result.setWidth(size.width());
      result.setHeight(image_.height() * (float)size.width() /
                       (float)image_.width());
    }

    return result;
  }

private:
  QPixmap image_;
  QPixmap image_to_paint_;
};

static const char *c_title_font_color_str = "#669692";
static const char *c_month_text_font_color_str = "#969692";

static const char *c_title_font_family_str = "Bellany";
static const char *c_month_text_font_family_str = "Areal";
static const char *c_title_defoult_text_str = "My First Year";

static const int c_title_text_font_size = 60;
static const int c_month_text_font_size = 20;

DefaultTemplateWidget::DefaultTemplateWidget(QWidget *parent,
                                             Core::FrameControl &control,
                                             bool render_state)

    : TemplateWidgetBase(
          parent,
          {control,
           templateId(),
           {{0, 25, 587, 75}, Qt::AlignCenter, c_title_font_family_str},
           {{{105, 242}, // month text center or left corner
             {233, 242},
             {362, 242},
             {492, 242},
             {105, 389},
             {233, 389},
             {362, 389},
             {492, 389},
             {105, 535},
             {233, 535},
             {362, 535},
             {492, 535}},
            Qt::AlignCenter,
            c_month_text_font_family_str},
           {{48, 122, 114, 114}, // month frame rect
            {176, 122, 114, 114},
            {305, 122, 114, 114},
            {435, 122, 114, 114},
            {48, 270, 114, 114},
            {176, 270, 114, 114},
            {305, 270, 114, 114},
            {435, 270, 114, 114},
            {48, 416, 114, 114},
            {176, 416, 114, 114},
            {305, 416, 114, 114},
            {435, 416, 114, 114}},
           {{FrameParameters::TYPE::RECT,
             QSizeF{114, 114}}}}, // month frame shape, if all are equal
          render_state) {

  // reload(control);
}

QString DefaultTemplateWidget::templateId() { return "1"; }

////////////////////////////////////////////////////////////////////
///
DefaultTemplateWidget2::DefaultTemplateWidget2(QWidget *parent,
                                               Core::FrameControl &control,
                                               bool render_state)

    : TemplateWidgetBase(parent,
                         {control,
                          templateId(),
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
                           {FrameParameters::TYPE::ROUND, QSizeF{125, 125}}}},
                         render_state) {

  // reload(control);
}

QString DefaultTemplateWidget2::templateId() { return "2"; }

////////////////////////////////////////////////////////////////////
///
TemplateWidgetBase::TemplateWidgetBase(
    QWidget *parent, const TemplateWidgetParameters &parameters,
    bool render_state)
    : QWidget(parent), id_(parameters.id),
      title_slot_real_(std::move(parameters.title_parameters.title_rect)),
      title_text_font_size_real_(c_title_text_font_size),
      photo_slots_real_(std::move(parameters.photo_slots)),
      photo_text_anchors_real_(
          std::move(parameters.photo_text_parameters.photo_text_anchors)),
      // photo_text_aligment_(parameters.photo_text_parameters.aligment),
      photo_text_font_size_real_(c_month_text_font_size),
      frame_data_(std::move(parameters.frame_data)),
      control_(parameters.control),
      title_font_(parameters.title_parameters.font),
      month_font_(parameters.photo_text_parameters.font),
      render_state_(render_state) {

  UNI_ASSERT(frame_data_.size() == 1 || frame_data_.size() == 12);
  UNI_ASSERT(photo_slots_real_.size() == 12);
  UNI_ASSERT(photo_text_anchors_real_.size() == 12);
  UNI_ASSERT(!id_.isEmpty());

  setContentsMargins(0, 0, 0, 0);
  setAutoFillBackground(true);

  foreground_ = QPixmap(
      render_state ? QString(c_foreground_to_render_str).arg(parameters.id)
                   : QString(c_foreground_str).arg(parameters.id));

  if (frame_data_.size() == 1) {
    frame_data_.resize(12, frame_data_[0]);
  }

  photo_slots_.resize(12);

  photo_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] = new PhotoWidget(*this, render_state_);
  }

  initMonthPhotoWidgets();

  createForegroundWidget();

  createTitleTextWidget(parameters.title_parameters.alignment, render_state);
  createPhotoTextWidgets(parameters.photo_text_parameters.alignment,
                         render_state);
  createRemoveButtonWidgets(render_state);
}

void TemplateWidgetBase::createTitleTextWidget(Qt::Alignment alignment,
                                               bool is_rendering) {
  title_text_widget_ =
      new ClickableLabel(this, c_title_text_font_size, c_title_font_color_str,
                         title_font_, is_rendering);

  title_text_widget_->setAlignment(alignment);
  connect(title_text_widget_, &ClickableLabel::clicked, this,
          [&] { emit SignalTitleClicked(title_text_widget_->text()); });
}

void TemplateWidgetBase::createRemoveButtonWidgets(bool is_rendering) {

  if (is_rendering) {
    return;
  }
  remove_buttons_.resize(12);
  for (int i = 0; i < (int)remove_buttons_.size(); i++) {
    remove_buttons_[i] = new QPushButton(this);
    remove_buttons_[i]->setIconSize({25, 25});
    remove_buttons_[i]->setIcon(QIcon(c_remove_image_str));
    remove_buttons_[i]->setAttribute(Qt::WA_TranslucentBackground);
    remove_buttons_[i]->setObjectName("RemoveuButton");

    connect(remove_buttons_[i], &ImageButton::clicked, this,
            [&, i] { emit SignalRemoveButtonClicked(i); });
  }
}

void TemplateWidgetBase::createPhotoTextWidgets(Qt::Alignment alignment, bool) {
  photo_text_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_text_widgets_[i] =
        new ClickableLabel(this, c_month_text_font_size,
                           c_month_text_font_color_str, month_font_, true);
    photo_text_widgets_[i]->setAlignment(alignment);

    //  connect(photo_text_widgets_[i], &ClickableLabel::clicked, this, [&, i] {
    //    emit SignalMonthTextClicked(photo_text_widgets_[i]->text(), i);
    //  });
  }
}

QString TemplateWidgetBase::id() const { return id_; }
QSize TemplateWidgetBase::preferedSize(QSize size) const {
  if (foreground_widget_) {
    return foreground_widget_->preferedSize(size);
  } else {
    return size;
  }
}

void TemplateWidgetBase::initMonthPhotoWidgets() {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    connect(photo_widgets_[i], &PhotoWidget::SignalImageDroped, this,
            [&, i](int dropped_index) {
              emit SignalImageDroped(dropped_index, i);
            });

    connect(photo_widgets_[i], &PhotoWidget::SignalImagePressed, this,
            [&, i] { emit SignalTunePhoto(i); });
  }
}

void TemplateWidgetBase::createForegroundWidget() {
  foreground_widget_ = new ForegroundWidget(this, foreground_);
  foreground_widget_->setGeometry(rect());
  foreground_widget_->raise();
  foreground_widget_->show();
  foreground_widget_->setContentsMargins(0, 0, 0, 0);
  foreground_widget_->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TemplateWidgetBase::Update() {

  foreground_widget_->setGeometry(rect());

  load(control_);

  title_text_widget_->setFontSize(title_text_font_size_);

  title_text_widget_->setGeometry(
      title_slot_.left(), title_slot_.top(), title_slot_.width(),
      title_text_widget_->heightForWidth(title_text_widget_->width()));

  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {

    auto &photo_text_widget = photo_text_widgets_[i];

    photo_text_widget->setFontSize(photo_text_font_size_);
    photo_text_widget->adjustSize();

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

void TemplateWidgetBase::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  Update();
}

void TemplateWidgetBase::load(Core::FrameControl &control) {

  double k = (double)width() / (double)foreground_.width();

  if (render_state_) {
    auto base_size = QImageReader(QString(c_foreground_str).arg(id_)).size();
    k *= (double)foreground_.width() / (double)base_size.width();
  }
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

  photo_text_font_size_ = photo_text_font_size_real_ * k;

  auto new_rect = title_slot_real_;
  new_rect.setTopLeft(title_slot_real_.topLeft() * k);
  new_rect.setSize(title_slot_real_.size() * k);
  title_slot_ = new_rect;

  title_text_font_size_ = title_text_font_size_real_ * k;

  InitPhotos(control);

  auto project = control.CurrentProject();
  title_text_widget_->setText(
      project->title_.isEmpty() ? c_title_defoult_text_str : project->title_);

  if (!render_state_) {
    UNI_ASSERT(remove_buttons_.size() == photo_slots_.size());
    for (int i = 0; i < (int)photo_slots_.size(); i++) {
      QRect new_rect;
      new_rect.setTopLeft(photo_slots_[i].topLeft().toPoint() -
                          QPoint(15.0, 10.0) * k);
      new_rect.setSize(QSize{(int)(50 * k), (int)(50 * k)});

      if (!project->monthes_[i].photo_data->isStub()) {
        remove_buttons_[i]->show();
        remove_buttons_[i]->setGeometry(new_rect);
        remove_buttons_[i]->setIconSize(new_rect.size());
      } else {
        remove_buttons_[i]->hide();
      }
    }
  }
}

void TemplateWidgetBase::InitPhotos(Core::FrameControl &control) {

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_widget = photo_widgets_[i];

    photo_widget->setGeometry(photo_slots_[i].toRect());
    photo_widget->setPhoto(month.photo_data, i);
    photo_widget->show();
  }

  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {
    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_text_widget = photo_text_widgets_[i];

    photo_text_widget->setText(month.text);
  }

  update();
  qDebug() << "update";
}

FrameParameters TemplateWidgetBase::frameData(int month) {
  UNI_ASSERT((int)frame_data_.size() > month);
  return frame_data_[month];
}

void TemplateWidgetBase::setVisible(bool visible) {
  if (visible) {
    load(control_);
  }
  QWidget::setVisible(visible);
}

QPixmap TemplateWidgetBase::renderFrame() {
  setGeometry({{0, 0}, foreground_.size()});
  QPixmap image = grab();

  const double dpr = QGuiApplication::primaryScreen()->devicePixelRatio();
  const int frame_with =
      std::min(image.size().width(), image.size().height()) / dpr / 14;
  const QSize frame_size{frame_with, frame_with};
  const QPoint image_top{frame_with, frame_with};
  const int k = 10;
  int shadow_with = frame_with / k;
  const QRect frame_rect = QRect{{}, image.size() / dpr + frame_size * 2};

  QPixmap result_image(frame_rect.size());
  QPainter painter(&result_image);

  painter.setBrush(QColor("#EBD9C7"));
  painter.setPen(QPen(QColor("#D7C4B4"), shadow_with / 2));

  painter.drawRect(frame_rect);
  painter.setPen(QColor("#EBD9C7"));
  painter.setBrush(QColor("#F2E6D3"));
  painter.drawRect(QRect{image_top - QPoint{shadow_with, shadow_with},
                         image.size() / dpr + frame_size * 2 / k});
  painter.drawPixmap(image_top, image);

  return result_image;
}

} // namespace FirstYear::UI
