#include "FrameDataReader.h"
#include <stdafx.h>

static const char *c_foreground_parameters_str = ":images/frame_%1/%1";

namespace FirstYear::UI {

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

bool FrameDataReader::readJson(const QString &id,
                               PhotoFrameParameters &parameters) {

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

} // namespace FirstYear::UI
