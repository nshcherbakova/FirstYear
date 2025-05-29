#include "FrameDataReader.h"
#include <stdafx.h>

static const char *c_foreground_parameters_str = ":frames/%1/properties";

namespace FirstYear::UI {

bool ReadTextParameters(const QJsonObject &object, TextParameters &parameters) {
  bool result = true;

  int alignment = 0;
  if (object.contains("alignment")) {
    result = result && Core::Json::ReadInt(object, "alignment", alignment);
    if (result) {
      parameters.alignment = (Qt::Alignment)alignment;
    }
  }

  if (object.contains("font")) {
    result = result && Core::Json::ReadString(object, "font", parameters.font);
  }
  if (object.contains("font_size")) {
    result = result &&
             Core::Json::ReadDouble(object, "font_size", parameters.font_size);
  }
  if (object.contains("font_color")) {
    result = result && Core::Json::ReadString(object, "font_color",
                                              parameters.font_color);
  }
  return result;
}

bool ReadTextParametersLocalized(const QJsonObject &object,
                                 TextParameters &parameters,
                                 const Core::Context &contex) {

  bool result = ReadTextParameters(object, parameters);

  if (object.contains(contex.locale())) {
    if (const auto data = object[contex.locale()]; data.isObject()) {
      result = result && ReadTextParameters(data.toObject(), parameters);
    }
  }

  return result;
}

bool readTitle(const QJsonObject &json, QString name,
               TitleParameters &parameters, const Core::Context &context) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;

    result =
        result && Core::Json::ReadRect(object, "rect", parameters.title_rect);

    result = result && ReadTextParametersLocalized(
                           object, parameters.text_parameters, context);

    return result;
  }
  return false;
}

bool ReadPhotoTextParameters(const QJsonObject &json,
                             PhotoTextParameters &parameters,
                             const Core::Context &context) {
  bool result = true;

  result =
      result && Core::Json::ReadPoint(json, "anchor", parameters.text_anchor);

  if (const auto data = json["text"]; data.isObject()) {
    const auto object = data.toObject();
    result = result && ReadTextParametersLocalized(
                           object, parameters.text_parameters, context);
  }
  return result;
}

bool readMonth(const QJsonObject &json, QString name,
               MonthParameters &parameters, const Core::Context &context) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;

    result = result && ReadPhotoTextParameters(
                           object, parameters.text_parameters, context);
    result =
        result && Core::Json::ReadRectF(object, "slot", parameters.photo_slot);

    if (const auto frame_data = object["frame_data"]; frame_data.isObject()) {
      const auto frame_data_object = frame_data.toObject();
      int type = 0;
      result = result && Core::Json::ReadInt(frame_data_object, "type", type);
      parameters.frame_data.type = (FrameParameters::TYPE)type;

      if (parameters.frame_data.type == FrameParameters::TYPE::RECT ||
          parameters.frame_data.type == FrameParameters::TYPE::ROUND) {
        QSizeF size;
        result =
            result && Core::Json::ReadSizeF(frame_data_object, "data", size);
        parameters.frame_data.data = size;
      }
    }

    return result;
  }
  return false;
}

bool FrameDataReader::readJson(const QString &id,
                               PhotoFrameParameters &parameters,
                               const Core::Context &context) {

  QFile file(QString(c_foreground_parameters_str).arg(id));
  if (!file.open(QIODevice::ReadOnly)) {
    spdlog::error("Couldn't open file {0}.", file.fileName().toStdString());
    return false;
  }
  const QByteArray data = file.readAll();
  file.close();

  const QJsonDocument document(QJsonDocument::fromJson(data));

  auto json = document.object();

  Core::Json::ReadString(json, "position", parameters.position);
  readTitle(json, "title", parameters.title_parameters, context);

  bool result = true;
  if (const auto data = json["mothes"]; data.isObject()) {
    const auto object = data.toObject();

    parameters.months_parameters.resize(12);
    for (int i = 0; i < 12; ++i) {
      result = result && readMonth(object, QString::number(i),
                                   parameters.months_parameters[i], context);
    }
  }
  return result;
}

} // namespace FirstYear::UI
