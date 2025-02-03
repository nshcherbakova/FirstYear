#pragma once
#ifndef PHOTO_FRAME_PARAMETERS_WIDGET_H
#define PHOTO_FRAME_PARAMETERS_WIDGET_H
#include <QPoint>
#include <QRect>
#include <QString>
#include <Types.h>
#include <UI/Utility.h>
#include <qnamespace.h>

namespace FirstYear::UI {

struct TextParameters {
  Qt::Alignment alignment;
  QString font;
  double font_size = 0;
  QString font_color;
};

struct TitleParameters {
  QRect title_rect;
  TextParameters text_parameters;
};

struct PhotoTextParameters {
  QPoint text_anchor;
  TextParameters text_parameters;
};

struct MonthParameters {
  PhotoTextParameters text_parameters;
  QRectF photo_slot;
  FrameParameters frame_data;
};

struct PhotoFrameParameters {
  TitleParameters title_parameters;
  std::vector<MonthParameters> months_parameters;
};

struct TemplateWidgetParameters {
  Core::FrameControl &control;
  QString id;
};

} // namespace FirstYear::UI
#endif
