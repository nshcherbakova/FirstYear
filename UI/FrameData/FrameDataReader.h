#pragma once
#ifndef FRAME_DATA_READER_H
#define FRAME_DATA_READER_H

#include <Types.h>
#include <UI/FrameData/PhotoFrameParameters.h>

namespace FirstYear::UI {

class FrameDataReader {

public:
  static bool readJson(const QString &id, PhotoFrameParameters &parameters,
                       const Core::Context &context);
};
} // namespace FirstYear::UI
#endif
