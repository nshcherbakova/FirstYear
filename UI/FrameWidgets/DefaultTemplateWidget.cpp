#include "DefaultTemplateWidget.h"
#include "PhotoWidget.h"
#include <UI/FrameData/FrameDataReader.h>
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_str = ":frames/%1/foreground";
static const char *c_foreground_to_render_str =
    ":frames/%1/foreground_to_render";

class ForegroundWidget final : public QWidget {
public:
  ForegroundWidget(QWidget *parent, QPixmap &image)
      : QWidget(parent), image_(image) {}

public:
  virtual void paintEvent(QPaintEvent *) override final {
    QPainter painter(this);

    painter.drawPixmap(rect(), image_to_paint_);
  }

  virtual void resizeEvent(QResizeEvent *e) override final {
    QWidget::resizeEvent(e);
    static const double dpr =
        QGuiApplication::primaryScreen()->devicePixelRatio();

    image_to_paint_ =
        image_.scaledToWidth(width() * dpr, Qt::SmoothTransformation);
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
  QPixmap &image_;
  QPixmap image_to_paint_;
};

////////////////////////////////////////////////////////////////////
///
///
///
/// class FrameWidgetsFactory

std::vector<TemplateWidgetBase *>
FrameWidgetsFactory::createWidgets(const QStringList &ids,
                                   Core::FrameControl &control) {
  std::vector<TemplateWidgetBase *> widgets;
  widgets.reserve(ids.size());
  for (const auto &id : ids) {
    widgets.push_back(new TemplateWidgetBase(nullptr, {control, id}));
  }
  return widgets;
}

TemplateWidgetBase *
FrameWidgetsFactory::createRearrangeWidget(Core::FrameControl &control) {
  return new TemplateWidgetBase(nullptr, {control, "Rearrange"});
}
QPixmap FrameWidgetsFactory::renderWidget(const QString &id, QWidget *parent,
                                          Core::FrameControl &control) {
  return TemplateWidgetBase(parent, {control, id},
                            TemplateWidgetBase::State::RENDER_FRAME)
      .renderFrame();
}

TemplateWidgetBase::TemplateWidgetBase(
    QWidget *parent, const TemplateWidgetParameters &widget_parameters,
    State render_state)
    : QWidget(parent), id_(widget_parameters.id),
      control_(widget_parameters.control), render_state_(render_state) {

  UNI_ASSERT(!id_.isEmpty());

  PhotoFrameParameters parameters;

  FrameDataReader::readJson(widget_parameters.id, parameters,
                            control_.context());
  fillParameters(parameters);

  setContentsMargins(0, 0, 0, 0);
  setAutoFillBackground(true);

  foreground_ = QPixmap(render_state_ == State::RENDER_FRAME
                            ? QString(c_foreground_to_render_str).arg(id_)
                            : QString(c_foreground_str).arg(id_));

  static const double dpr =
      QGuiApplication::primaryScreen()->devicePixelRatio();
  foreground_.setDevicePixelRatio(dpr);

  photo_slots_.resize(12);
  photo_widgets_.resize(12);

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] =
        new PhotoWidget(*this, {false, false, render_state_ == State::FRAME,
                                render_state_ == State::FRAME, true});
  }

  initMonthPhotoWidgets();

  createForegroundWidget();
  createTitleTextWidget(parameters.title_parameters.text_parameters,
                        render_state_ == State::RENDER_FRAME);

  createPhotoTextWidgets(parameters.months_parameters);
}

void TemplateWidgetBase::fillParameters(
    const PhotoFrameParameters &parameters) {
  UNI_ASSERT(parameters.months_parameters.size() == 12);

  position_ = parameters.position;

  title_slot_real_ = std::move(parameters.title_parameters.title_rect);
  title_text_font_size_real_ =
      parameters.title_parameters.text_parameters.font_size;

  photo_text_font_size_real_ =
      parameters.months_parameters[0].text_parameters.text_parameters.font_size;

  photo_slots_real_.resize(12);
  frame_data_.resize(12);
  photo_text_anchors_real_.resize(12);
  for (int i = 0; i < 12; i++) {
    const auto &months_parameters = parameters.months_parameters[i];

    photo_slots_real_[i] = months_parameters.photo_slot;
    frame_data_[i] = months_parameters.frame_data;
    photo_text_anchors_real_[i] = months_parameters.text_parameters.text_anchor;
  }
}

void TemplateWidgetBase::createTitleTextWidget(const TextParameters &parameters,
                                               bool is_rendering) {
  title_text_widget_ =
      new ClickableLabel(this, parameters.font_size, parameters.font_color,
                         parameters.font, is_rendering, true);

  title_text_widget_->setAlignment(parameters.alignment);
  connect(title_text_widget_, &ClickableLabel::clicked, this,
          [&] { emit SignalTitleClicked(title_text_widget_->text()); });
}

void TemplateWidgetBase::createPhotoTextWidgets(
    const std::vector<MonthParameters> &month_parameters) {
  const auto &parameters_fixed =
      month_parameters[0].text_parameters.text_parameters;

  photo_text_widgets_.resize(12);
  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {
    const auto &parameters =
        month_parameters[i].text_parameters.text_parameters;

    auto &widget = photo_text_widgets_[i];

    widget = new ClickableLabel(this, parameters_fixed.font_size,
                                parameters_fixed.font_color,
                                parameters_fixed.font, true);
    widget->setAlignment(parameters.alignment);
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
  }
}

QString TemplateWidgetBase::id() const { return id_; }

QString TemplateWidgetBase::position() const { return position_; }

QSize TemplateWidgetBase::preferedSize(QSize size) const {
  if (foreground_widget_) {
    return foreground_widget_->preferedSize(size);
  } else {
    return size;
  }
}

void TemplateWidgetBase::initMonthPhotoWidgets() {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

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

  load(control_);

  foreground_widget_->setGeometry(rect());
  title_text_widget_->setGeometry(title_slot_.left(), title_slot_.top(),
                                  title_slot_.width(), title_slot_.height());

  title_text_widget_->setFontSize(title_text_font_size_);

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

  auto new_size = e->size();
  QMetaObject::invokeMethod(
      this,
      [&, new_size]() {
        if (new_size != image_buffer_.size()) {
          image_buffer_ = grab();
        }
      },
      Qt::QueuedConnection);
}

void TemplateWidgetBase::load(Core::FrameControl &control) {

  double k = (double)width() / (double)foreground_.width();

  if (render_state_ == State::RENDER_FRAME) {
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
  title_text_widget_->setText(project->title_.isEmpty() ? tr("My First Year")
                                                        : project->title_);
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
      std::min(image.size().width(), image.size().height()) / dpr / 25;
  const QSize frame_size{frame_with, frame_with};
  const QPoint image_top{frame_with, frame_with};
  const int k = 10;
  int shadow_with = frame_with / k / 2;
  const QRect frame_rect = QRect{{}, image.size() / dpr + frame_size * 2};

  QPixmap result_image(frame_rect.size());
  QPainter painter(&result_image);

  // frame
  painter.setBrush(QColor("#f0eee5"));
  painter.setPen(QPen(QColor("#f0eee5"), shadow_with / 2));
  painter.drawRect(frame_rect);

  // frame pattern
  // auto svg = new QSvgRenderer(this);
  // svg->load(QString(":/images/icons/wood"));
  // svg->render(&painter, frame_rect);

  // inner shaddow
  painter.setPen(QColor("#cecec8"));
  painter.setBrush(QColor("#cecec8"));
  painter.drawRect(QRect{image_top - QPoint{shadow_with, shadow_with},
                         image.size() / dpr + frame_size / k});

  painter.drawPixmap(image_top, image);

  return result_image;
}

void TemplateWidgetBase::paintEvent(QPaintEvent *) {

  // QWidget::paintEvent(e);
  // return;
  QPainter painter(this);

  painter.drawPixmap(rect(), image_buffer_);
}

} // namespace FirstYear::UI
