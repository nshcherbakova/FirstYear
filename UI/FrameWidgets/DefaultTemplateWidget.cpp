#include "DefaultTemplateWidget.h"
#include "PhotoWidget.h"
#include <UI/FrameData/FrameDataReader.h>
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_str = ":frames/%1/foreground";
static const char *c_foreground_to_render_str =
    ":frames/%1/foreground_to_render";
// static const int c_max_title_lengh = 20;

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

static const char *c_title_defoult_text_str = "My First Year";

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

QPixmap FrameWidgetsFactory::renderWidget(const QString &id, QWidget *parent,
                                          Core::FrameControl &control) {
  return TemplateWidgetBase(parent, {control, id}, true).renderFrame();
}

TemplateWidgetBase::TemplateWidgetBase(
    QWidget *parent, const TemplateWidgetParameters &widget_parameters,
    bool render_state)
    : QWidget(parent), id_(widget_parameters.id),
      control_(widget_parameters.control), render_state_(render_state) {

  UNI_ASSERT(!id_.isEmpty());

  PhotoFrameParameters parameters;

  FrameDataReader::readJson(widget_parameters.id, parameters);
  fillParameters(parameters);

  setContentsMargins(0, 0, 0, 0);
  setAutoFillBackground(true);

  foreground_ =
      QPixmap(render_state ? QString(c_foreground_to_render_str).arg(id_)
                           : QString(c_foreground_str).arg(id_));

  static const double dpr =
      QGuiApplication::primaryScreen()->devicePixelRatio();
  foreground_.setDevicePixelRatio(dpr);

  photo_slots_.resize(12);

  photo_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] = new PhotoWidget(*this, render_state_);
  }

  initMonthPhotoWidgets();

  createForegroundWidget();

  createTitleTextWidget(parameters.title_parameters.text_parameters,
                        render_state);
  createPhotoTextWidgets(
      parameters.months_parameters[0].text_parameters.text_parameters,
      render_state);
  createRemoveButtonWidgets(render_state);
}

void TemplateWidgetBase::fillParameters(
    const PhotoFrameParameters &parameters) {
  UNI_ASSERT(parameters.months_parameters.size() == 12);

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
                         parameters.font, is_rendering);

  title_text_widget_->setAlignment(parameters.alignment);
  connect(title_text_widget_, &ClickableLabel::clicked, this,
          [&] { emit SignalTitleClicked(title_text_widget_->text()); });
  // title_text_widget_->setVisible(false);
}

void TemplateWidgetBase::createRemoveButtonWidgets(bool is_rendering) {

  if (is_rendering) {
    return;
  }

  remove_buttons_.resize(12);
  for (int i = 0; i < (int)remove_buttons_.size(); i++) {
    remove_buttons_[i] = new QPushButton(this);
    remove_buttons_[i]->setIconSize({40, 40});
    remove_buttons_[i]->setIcon(QIcon(c_remove_image_str));
    remove_buttons_[i]->setAttribute(Qt::WA_TranslucentBackground);
    remove_buttons_[i]->setObjectName("RemoveuButton");

    connect(remove_buttons_[i], &ImageButton::clicked, this,
            [&, i] { emit SignalRemoveButtonClicked(i); });
  }
}

void TemplateWidgetBase::createPhotoTextWidgets(
    const TextParameters &parameters, bool) {
  photo_text_widgets_.resize(12);
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_text_widgets_[i] =
        new ClickableLabel(this, parameters.font_size, parameters.font_color,
                           parameters.font, true);
    photo_text_widgets_[i]->setAlignment(parameters.alignment);

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
                          QPoint(15.0, 15.0) * k);
      new_rect.setSize(QSize{(int)(70 * k), (int)(70 * k)});

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

  auto svg = new QSvgRenderer(this);
  svg->load(QString(":/images/icons/wood"));
  svg->render(&painter, frame_rect);

  painter.drawPixmap(image_top, image);

  return result_image;
}

} // namespace FirstYear::UI
