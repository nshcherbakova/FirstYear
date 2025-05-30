#pragma once
#ifndef FIRSTYEAR_CORE_TYPES_H
#define FIRSTYEAR_CORE_TYPES_H
#include <QTypes.h>
#include <memory>

namespace FirstYear::Core {
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class PhotoData;
using PhotoDataPtr = std::shared_ptr<PhotoData>;
class FrameControl;
class ImageManager;
using ImageManagerPtr = std::shared_ptr<ImageManager>;
class Context;
} // namespace FirstYear::Core

namespace FirstYear::UI {
class PhotoWidget;
class PhotoTuneWidget;
class SwipeWidgetsList;
} // namespace FirstYear::UI

#endif // FIRSTYEAR_CORE_TYPES_H
