#include "PhotoProcessor.h"
#include <stdafx.h>

namespace FirstYear::UI {
const double MIN_SIZE_K = 0.7;
const double MAX_SIZE_K = 15.0;
const double INITIAL_SCALE_FACTOR = 2.5;

PhotoPainter::PhotoPainter()
    : dpr_(QGuiApplication::primaryScreen()->devicePixelRatio()) {}

void PhotoPainter::init(const Core::PhotoDataPtr &photo,
                        QRectF destanation_rect, QRectF boundary_rect) {

  photo_data_ = photo;
  boundary_rect_ = boundary_rect;
  destanation_rect_ = destanation_rect;

  const QRectF photo_rect = {QPointF(0, 0), photo_data_->image().size() / dpr_};

  const double k1 = photo_rect.width() / photo_rect.height();
  const double k2 = scaleRefRect().width() / scaleRefRect().height();

  if (k1 < k2) {
    internal_scale_ = ((double)scaleRefRect().height() / photo_rect.height()) *
                      initialScaleFactor();
  } else {
    internal_scale_ = ((double)scaleRefRect().width() / photo_rect.width()) *
                      initialScaleFactor();
  }

  transform_ = getTransformForWidget(
      {std::optional<double>(), std::optional<double>(),
       std::optional<QPointF>(), std::optional<QPointF>()},
      photo_data_->transformOffsetRef(),
      photo_data_->transformScaleRotateRef());
}

double PhotoPainter::initialScaleFactor() const { return INITIAL_SCALE_FACTOR; }

QRectF PhotoPainter::scaleRefRect() const { return boundary_rect_; }

QTransform PhotoPainter::getTransformForWidget(
    const PhotoPosition &photo_position, Core::PhotoTransform &transform_offset,
    Core::PhotoTransform &transform_scale_rotate) {
  // spdlog::info("a {}, s {},  ", photo_position_.angle, photo_.scale);
  const auto angle_diff = photo_position.angle.value_or(0);
  const auto scale_diff = photo_position.scale.value_or(1);
  const auto offset_diff = photo_position.offset.value_or(QPointF(0, 0));
  const auto center = photo_position.center.value_or(QPointF(0, 0));

  const qreal iw = photo_data_->image().width() / dpr_;
  const qreal ih = photo_data_->image().height() / dpr_;
  const qreal wh = destanation_rect_.height();
  const qreal ww = destanation_rect_.width();

  QPointF d = {destanation_rect_.left() + ww / 2,
               destanation_rect_.top() + wh / 2};

  QTransform transform_internal1;
  transform_internal1.translate(d.x(), d.y());
  transform_internal1.scale(internal_scale_, internal_scale_);

  if (photo_position.offset.has_value()) {
    transform_offset.translate(offset_diff.x() / internal_scale_,
                               offset_diff.y() / internal_scale_);
  }

  if (photo_position.angle.has_value() || photo_position.scale.has_value() ||
      photo_position.center.has_value()) {

    QPointF c =
        (transform_scale_rotate * transform_offset * transform_internal1)
            .inverted()
            .map(center);

    transform_scale_rotate.translate(c.x(), c.y());
    transform_scale_rotate.rotate(angle_diff);
    transform_scale_rotate.scale(scale_diff, scale_diff);
    transform_scale_rotate.translate(-c.x(), -c.y());
  }

  QTransform transform_internal2;
  transform_internal2.translate(-iw / 2, -ih / 2);

  return transform_internal2 * transform_scale_rotate * transform_offset *
         transform_internal1;
}

void PhotoPainter::drawPhoto(QPainter &painter) {
  if (!photo_data_ || photo_data_->image().isNull())
    return;

  drawFrame(painter);

  painter.setTransform(transform_);
  painter.drawPixmap(0, 0, photo_data_->image());
  painter.setTransform(QTransform());
}

void PhotoPainter::drawFrame(QPainter &) {
  /* int margin = 12;
   auto boder_rect =
       QRect{QPoint{margin, margin},
             photo_data_->image().size() /
                 QGuiApplication::primaryScreen()->devicePixelRatio()};

   frame_ = QPixmap(boder_rect.size() + QSize{margin, margin});

   QPainter painter(&frame_);

   auto old_pen = painter.pen();
   auto pen = old_pen;
   pen.setColor(QColor(150, 150, 150, 10));
   pen.setWidth(0);
   painter.setPen(pen);
   auto old_brush = painter.brush();
   painter.setBrush(QColor(150, 150, 150, 30));

   painter.drawRoundedRect(boder_rect, 4, 4);

   painter.setBrush(old_brush);
   painter.setPen(old_pen);*/
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///
///
bool PhotoProcessor::checkBoundares(const QTransform &transform) const {

  const QRectF image_rect = {QPointF(0, 0), photo_data_->image().size() / dpr_};

  const auto translated_image_rect = transform.mapRect(image_rect);

  if (translated_image_rect.width() < boundary_rect_.width() * MIN_SIZE_K ||
      translated_image_rect.height() < boundary_rect_.height() * MIN_SIZE_K)
    return false;

  if (translated_image_rect.width() > boundary_rect_.width() * MAX_SIZE_K ||
      translated_image_rect.height() > boundary_rect_.height() * MAX_SIZE_K)
    return false;

  if (!translated_image_rect.intersects(boundary_rect_))
    return false;

  return true;
}

void PhotoProcessor::updatePhotoPosition(std::optional<QPointF> pos_delta,
                                         std::optional<double> scale_factor,
                                         std::optional<double> angle_delta,
                                         std::optional<QPointF> center) {
  auto scale_rotate = photo_data_->transformScaleRotate();
  auto translate = photo_data_->transformOffset();
  auto transform = getTransformForWidget(
      {angle_delta, scale_factor, pos_delta, center}, translate, scale_rotate);

  if (checkBoundares(transform)) {
    transform_ = transform;
    photo_data_->setTransforms(std::move(scale_rotate), std::move(translate));
  }
}

} // namespace FirstYear::UI
