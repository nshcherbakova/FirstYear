#pragma once
#ifndef FIRSTYEAR_UI_UTILITY_H
#define FIRSTYEAR_UI_UTILITY_H
#include <QString>
#include <QStringList>
#include <QVariant>

class QWidget;

namespace FirstYear::UI {
struct FrameParameters {
  enum class TYPE {
    ROUND,
    RECT,
  };
  TYPE type;
  QVariant data;
};
} // namespace FirstYear::UI
Q_DECLARE_METATYPE(FirstYear::UI::FrameParameters)

namespace FirstYear::UI::Utility {
QString OpenFile(QWidget *parent);
QStringList OpenFiles(QWidget *parent);
} // namespace FirstYear::UI::Utility
#endif // FIRSTYEAR_UI_UTILITY_H
