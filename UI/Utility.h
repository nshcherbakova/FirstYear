#pragma once
#ifndef FIRSTYEAR_UI_UTILITY_H
#define FIRSTYEAR_UI_UTILITY_H
#include <QString>
#include <QStringList>
#include <QVariant>
#ifdef Q_OS_ANDROID
#include <Core/ImagePicker/ImagePicker.h>
#endif

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
// QString static RandomBackground();
} // namespace FirstYear::UI::Utility
#endif // FIRSTYEAR_UI_UTILITY_H
