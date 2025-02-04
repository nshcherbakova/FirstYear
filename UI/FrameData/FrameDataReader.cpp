#include "FrameDataReader.h"
#include <stdafx.h>

static const char *c_foreground_parameters_str = ":frames/%1/properties";

namespace FirstYear::UI {

bool ReadTextParameters(const QJsonObject &object, TextParameters &parameters) {
  bool result = true;

  int alignment = 0;
  result = result && Core::Json::ReadInt(object, "alignment", alignment);
  parameters.alignment = (Qt::Alignment)alignment;

  result = result && Core::Json::ReadString(object, "font", parameters.font);

  result = result &&
           Core::Json::ReadDouble(object, "font_size", parameters.font_size);

  result = result &&
           Core::Json::ReadString(object, "font_color", parameters.font_color);

  return result;
}

bool readTitle(const QJsonObject &json, QString name,
               TitleParameters &parameters) {
  if (const auto data = json[name]; data.isObject()) {
    const auto object = data.toObject();

    bool result = true;

    result =
        result && Core::Json::ReadRect(object, "rect", parameters.title_rect);

    result = result && ReadTextParameters(object, parameters.text_parameters);

    return result;
  }
  return false;
}

bool ReadPhotoTextParameters(const QJsonObject &json,
                             PhotoTextParameters &parameters) {
  bool result = true;

  result =
      result && Core::Json::ReadPoint(json, "anchor", parameters.text_anchor);

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
