#pragma once
#ifndef FIRSTYEAR_CORE_TYPES_H
#define FIRSTYEAR_CORE_TYPES_H
#include <QTypes.h>
#include <memory>

namespace FirstYear::Core {
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class FrameControl;
} // namespace FirstYear::Core

namespace FirstYear::UI {
class PhotoWidget;
class PhotoTuneWidget;
} // namespace FirstYear::UI

#endif // FIRSTYEAR_CORE_TYPES_H
