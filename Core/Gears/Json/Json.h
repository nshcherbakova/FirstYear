#pragma once
#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <QTypes.h>

namespace FirstYear::Core::Json {
bool ReadString(const QJsonObject &json, QString key, QString &value);
bool ReadInt(const QJsonObject &json, QString key, int &value);
bool ReadDouble(const QJsonObject &json, QString key, double &value);
bool ReadSizeF(const QJsonObject &json, QString key, QSizeF &value);
bool ReadPoint(const QJsonObject &json, QString key, QPoint &value);
bool ReadRect(const QJsonObject &obj, QString name, QRect &rect);
bool ReadRectF(const QJsonObject &obj, QString name, QRectF &rect);
} // namespace FirstYear::Core::Json
#endif
