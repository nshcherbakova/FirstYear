#pragma once
#ifndef PHOTO_PROCESSOR_H
#define PHOTO_PROCESSOR_H
#include <Core/Project/Project.h>
#include <QPixmap>
#include <QWidget>
#include <UI/FrameWidgets/Touch/GestureProcessor.h>
#include <UI/Utility.h>

namespace FirstYear::UI {

/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoPainter class
///

class PhotoPainter {
public:
  PhotoPainter();

public:
  PhotoPainter &operator=(const PhotoPainter &) = delete;

public:
  void init(const Core::PhotoDataPtr &photo, QRectF destanation_rect,
            QRectF boundary_rect);
  void drawPhoto(QPainter &);

protected:
  virtual double initialScaleFactor() const;
  virtual void drawFrame(QPainter &painter);
  virtual QRectF scaleRefRect() const;

protected:
  struct PhotoPosition {
    std::optional<double> angle;
    std::optional<double> scale;
    std::optional<QPointF> offset;
    std::optional<QPointF> center;

    void reset() {
      offset.reset();
      scale.reset();
      angle.reset();
      center.reset();
    }

    void reset_exept_center() {
      offset.reset();
      scale.reset();
      angle.reset();
    }
  };
  QTransform
  getTransformForWidget(const PhotoPosition &photo_position,
                        Core::PhotoTransform &transform_offset,
                        Core::PhotoTransform &transform_scale_rotate);

protected:
  QTransform transform_;
  Core::PhotoDataPtr photo_data_;
  QPixmap frame_;

  double internal_scale_ = 1;
  QRectF boundary_rect_;
  QRectF destanation_rect_;

  const double dpr_;
};
/////////////////////////////////////////////////////////////////////////////
/// \brief The PhotoProcessor class
///

class PhotoProcessor : protected PhotoPainter {
public:
  PhotoProcessor &operator=(const PhotoProcessor &) = delete;

protected:
  void updatePhotoPosition(std::optional<QPointF> pos_delta,
                           std::optional<double> scale_factor,
                           std::optional<double> angle_delta,
                           std::optional<QPointF> center);

private:
  bool checkBoundares(const QTransform &transform) const;
};

} // namespace FirstYear::UI
#endif
