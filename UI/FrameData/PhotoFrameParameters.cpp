#include "DefaultTemplateWidget.h"
#include "PhotoWidget.h"
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_foreground_parameters_str = ":images/frame_%1/%1";
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

static const char *c_title_defoult_text_str = "My First Year";

DefaultTemplateWidget::DefaultTemplateWidget(QWidget *parent,
                                             Core::FrameControl &control,
                                             bool render_state)

    : TemplateWidgetBase(
          parent,
          {control, templateId()}, // month frame shape, if all are equal
          render_state) {

  // reload(control);
}

QString DefaultTemplateWidget::templateId() { return "1"; }

////////////////////////////////////////////////////////////////////
///
DefaultTemplateWidget2::DefaultTemplateWidget2(QWidget *parent,
                                               Core::FrameControl &control,
                                               bool render_state)

    : TemplateWidgetBase(parent, {control, templateId()}, render_state) {

  // reload(control);
}

QString DefaultTemplateWidget2::templateId() { return "2"; }

////////////////////////////////////////////////////////////////////
///
///

void writeRect(QJsonObject &obj, QString name, QRect rect) {
  QJsonObject rect_obj;
  obj.insert(name, rect_obj);
  rect_obj.insert("left", rect.left());
  rect_obj.insert("top", rect.top());
  rect_obj.insert("right", rect.right());
  rect_obj.insert("bottom", rect.bottom());
}

void writeRect(QJsonObject &obj, QString name, QRectF rect) {
  QJsonObject rect_obj;

  rect_obj.insert("left", rect.left());
  rect_obj.insert("top", rect.top());
  rect_obj.insert("right", rect.right());
  rect_obj.insert("bottom", rect.bottom());
  obj.insert(name, rect_obj);
}

void writeJson(const TemplateWidgetParameters &) {
  /*   QJsonObject frame;
     QJsonObject title;
     QJsonObject mothes;
     QJsonObject rect;

     writeRect(title, "rect", parameters.title_parameters.title_rect);
     title.insert("alignment", (short)parameters.title_parameters.alignment);
     title.insert("font", parameters.title_parameters.font);
     title.insert("font_size", parameters.title_parameters.font_size);
     title.insert("font_color", parameters.title_parameters.font_color);

     for (int i = 0; i < 12; ++i) {
         QJsonObject month;

         QJsonObject text;
         text.insert("alignment",
     (short)parameters.photo_text_parameters.alignment); text.insert("font",
     parameters.photo_text_parameters.font); text.insert("font_size",
     parameters.photo_text_parameters.font_size); text.insert("font_color",
     parameters.photo_text_parameters.font_color); month.insert("text", text);

         QJsonObject anchor;
         anchor.insert("x",
     parameters.photo_text_parameters.photo_text_anchors[i].x());
         anchor.insert("y",
     parameters.photo_text_parameters.photo_text_anchors[i].y());
         month.insert("anchor",  anchor);

         QJsonObject slot;
         writeRect(slot, "rect", parameters.photo_slots[i]);
         month.insert("slot",  slot);

         QJsonObject frame_data;

         const  FrameParameters& params = parameters.frame_data.size() > 1?
     parameters.frame_data[i] : parameters.frame_data[0];
         frame_data.insert("type", (int)params.type);
         month.insert("frame_data",  frame_data);

         QJsonObject data;
         month.insert("data", params.data.toJsonValue());
         mothes.insert(QString("%1").arg(i),  month);

     }
     frame.insert("id",  parameters.id);
     frame.insert("title",  title);
     frame.insert("mothes",  mothes);
     QJsonDocument project_metadata_document(frame);

     QFile file
     (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
     "/FirstYear/FrameData/"+parameters.id); file.open(QIODevice::WriteOnly);
     file.write(project_metadata_document.toJson());
     file.close();*/
}

bool ReadString(const QJsonObject &json, QString key, QString &value) {
  if (const QJsonValue json_value = json[key]; json_value.isString()) {
    value = json_value.toString();
    return true;
  }

  spdlog::error("Error while reading a string from json. {}",
                key.toStdString());
  return false;
}

bool ReadInt(const QJsonObject &json, QString key, int &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toInt();
    return true;
  }

  spdlog::error("Error while reading an int from json. {}", key.toStdString());
  return false;
}

bool ReadDouble(const QJsonObject &json, QString key, double &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toDouble();
    return true;
  }

  spdlog::error("Error while reading a double from json. {}",
                key.toStdString());
  return false;
}

bool ReadSizeF(const QJsonObject &json, QString key, QSizeF &value) {
  if (const auto data = json[key]; data.isObject()) {
    const auto object = data.toObject();
    bool result = ReadDouble(object, "width", value.rwidth());
    result = result && ReadDouble(object, "height", value.rheight());
    return result;
  }

  spdlog::error("Error while reading an QSizeF from json. {}",
                key.toStdString());
  return false;
}

bool ReadPoint(const QJsonObject &json, QString key, QPoint &value) {
  if (const auto data = json[key]; data.isObject()) {
    const auto object = data.toObject();
    bool result = ReadInt(object, "x", value.rx());
    result = result && ReadInt(object, "y", value.ry());
    return result;
  }

  spdlog::error("Error while reading an QPoint from json. {}",
                key.toStdString());
  return false;
}

bool ReadRect(const QJsonObject &obj, QString name, QRect &rect) {
  bool result = true;
  if (const auto data = obj[name]; data.isObject()) {
    const auto rect_obj = data.toObject();

    int left = 0;
    result = result && ReadInt(rect_obj, "left", left);
    int top = 0;
    result = result && ReadInt(rect_obj, "top", top);
    int right = 0;
    result = result && ReadInt(rect_obj, "right", right);
    int bottom = 0;
    result = result && ReadInt(rect_obj, "bottom", bottom);

    rect.setBottomRight({right, bottom});
    rect.setTopLeft({left, top});
    return result;
  }
  return false;
}

bool ReadRectF(const QJsonObject &obj, QString name, QRectF &rect) {
  bool result = true;
  if (const auto data = obj[name]; data.isObject()) {
    const auto rect_obj = data.toObject();

    double left = 0;
    result = result && ReadDouble(rect_obj, "left", left);
    double top = 0;
    result = result && ReadDouble(rect_obj, "top", top);
    double right = 0;
    result = result && ReadDouble(rect_obj, "right", right);
    double bottom = 0;
    result = result && ReadDouble(rect_obj, "bottom", bottom);

    rect.setBottomRight({right, bottom});
    rect.setTopLeft({left, top});
    return result;
  }
  return false;
}

bool ReadTextParameters(const QJsonObject &object, TextParameters &parameters) {
  bool result = true;

  int alignment = 0;
  result = result && ReadInt(object, "alignment", alignment);
  parameters.alignment = (Qt::Alignment)alignment;

  result = result && ReadString(object, "font", parameters.font);

  result = result && ReadDouble(object, "font_size", parameters.font_size);

  result = result && ReadString(object, "font_color", parameters.font_color);

  return result;
}

bool readTitle(const QJsonObject &json, QString name,
               TitleParameters &parameters) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;

    result = result && ReadRect(object, "rect", parameters.title_rect);

    result = result && ReadTextParameters(object, parameters.text_parameters);

    return result;
  }
  return false;
}

bool ReadPhotoTextParameters(const QJsonObject &json,
                             PhotoTextParameters &parameters) {
  bool result = true;

  result = result && ReadPoint(json, "anchor", parameters.text_anchor);

  if (const auto data = json["text"]; data.isObject()) {
    const auto object = data.toObject();
    result = result && ReadTextParameters(object, parameters.text_parameters);
  }
  return result;
}

bool readMonth(const QJsonObject &json, QString name,
               MonthParameters &parameters) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;

    result =
        result && ReadPhotoTextParameters(object, parameters.text_parameters);
    result = result && ReadRectF(object, "slot", parameters.photo_slot);

    if (const auto frame_data = object["frame_data"]; frame_data.isObject()) {
      const auto frame_data_object = frame_data.toObject();
      int type = 0;
      result = result && ReadInt(frame_data_object, "type", type);
      parameters.frame_data.type = (FrameParameters::TYPE)type;

      if (parameters.frame_data.type == FrameParameters::TYPE::RECT ||
          parameters.frame_data.type == FrameParameters::TYPE::ROUND) {
        QSizeF size;
        result = result && ReadSizeF(frame_data_object, "data", size);
        parameters.frame_data.data = size;
      }
    }

    return result;
  }
  return false;
}

bool readJson(const QString &id, PhotoFrameParameters &parameters) {

  QFile file(QString(c_foreground_parameters_str).arg(id));
  if (!file.open(QIODevice::ReadOnly)) {
    spdlog::error("Couldn't open file {0}.", file.fileName().toStdString());
    return false;
  }
  const QByteArray data = file.readAll();
  file.close();

  const QJsonDocument document(QJsonDocument::fromJson(data));

  auto json = document.object();

  readTitle(json, "title", parameters.title_parameters);

  bool result = true;
  if (const auto data = json["mothes"]; data.isObject()) {
    const auto object = data.toObject();

    parameters.months_parameters.resize(12);
    for (int i = 0; i < 12; ++i) {
      result = result && readMonth(object, QString::number(i),
                                   parameters.months_parameters[i]);
    }
  }
  return result;
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
TemplateWidgetBase::TemplateWidgetBase(
    QWidget *parent, const TemplateWidgetParameters &widget_parameters,
    bool render_state)
    : QWidget(parent), id_(widget_parameters.id),
      control_(widget_parameters.control), render_state_(render_state) {

  UNI_ASSERT(!id_.isEmpty());

  PhotoFrameParameters parameters;

  readJson(widget_parameters.id, parameters);
  fillParameters(parameters);

  setContentsMargins(0, 0, 0, 0);
  setAutoFillBackground(true);

  foreground_ =
      QPixmap(render_state ? QString(c_foreground_to_render_str).arg(id_)
                           : QString(c_foreground_str).arg(id_));

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

void TemplateWidgetBase::createTitleTextWidget(const TextParameters &parameters,
                                               bool is_rendering) {
  title_text_widget_ =
      new ClickableLabel(this, parameters.font_size, parameters.font_color,
                         parameters.font, is_rendering);

  title_text_widget_->setAlignment(parameters.alignment);
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

  auto svg = new QSvgRenderer(this);
  svg->load(QString(":/images/icons/wood"));
  svg->render(&painter, frame_rect);

  painter.drawPixmap(image_top, image);

  return result_image;
}

} // namespace FirstYear::UI
