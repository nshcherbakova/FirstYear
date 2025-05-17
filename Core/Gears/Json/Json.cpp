#include "Json.h"
#include <stdafx.h>

namespace FirstYear::Core::Json {

bool ReadString(const QJsonObject &json, QString key, QString &value) {
  if (const QJsonValue json_value = json[key]; json_value.isString()) {
    value = json_value.toString();
    return true;
  }

  spdlog::error("Error while reading a string from json {}", key.toStdString());
  return false;
}

bool ReadInt(const QJsonObject &json, QString key, int &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toInt();
    return true;
  }

  spdlog::error("Error while reading an int from json {}", key.toStdString());
  return false;
}

bool ReadDouble(const QJsonObject &json, QString key, double &value) {
  if (const QJsonValue json_value = json[key]; json_value.isDouble()) {
    value = json_value.toDouble();
    return true;
  }

  spdlog::error("Error while reading a double from json {}", key.toStdString());
  return false;
}

bool ReadSizeF(const QJsonObject &json, QString key, QSizeF &value) {
  if (const auto data = json[key]; data.isObject()) {
    const auto object = data.toObject();
    bool result = ReadDouble(object, "width", value.rwidth());
    result = result && ReadDouble(object, "height", value.rheight());
    return result;
  }

  spdlog::error("Error while reading an QSizeF from json {}",
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

  spdlog::error("Error while reading an QPoint from json {}",
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
    int width = 0;
    result = result && ReadInt(rect_obj, "width", width);
    int height = 0;
    result = result && ReadInt(rect_obj, "height", height);

    rect.setTopLeft({left, top});
    rect.setSize({width, height});
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
    double width = 0;
    result = result && ReadDouble(rect_obj, "width", width);
    double height = 0;
    result = result && ReadDouble(rect_obj, "height", height);

    rect.setTopLeft({left, top});
    rect.setSize({width, height});
    return result;
  }
  return false;
}

} // namespace FirstYear::Core::Json
