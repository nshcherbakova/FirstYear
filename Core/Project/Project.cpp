#include "Project.h"
#include <stdafx.h>

// static const int c_image_scale_size = 1536;

namespace FirstYear::Core {
ImageManagerPtr PhotoData::image_manager_;

bool absoluteToleranceCompare(double x, double y) {
  return std::fabs(x - y) <= std::numeric_limits<double>::epsilon();
}

PhotoTransform::PhotoTransform(PhotoData *owner, short state_flag)
    : owner_(owner), state_flag_(state_flag) {}

void PhotoTransform::reset() {
  QTransform::reset();
  owner_->setState(owner_->state() | state_flag_);
}

QTransform &PhotoTransform::rotate(qreal a, Qt::Axis axis) {
  if (!absoluteToleranceCompare(a, 0)) {
    owner_->setState(owner_->state() | state_flag_);
  }
  return QTransform::rotate(a, axis);
}

QTransform &PhotoTransform::scale(qreal sx, qreal sy) {
  if (!absoluteToleranceCompare(sx, 1) || !absoluteToleranceCompare(sy, 1)) {
    owner_->setState(owner_->state() | state_flag_);
  }
  return QTransform::scale(sx, sy);
}

QTransform &PhotoTransform::translate(qreal dx, qreal dy) {
  if (!absoluteToleranceCompare(dx, 0) || !absoluteToleranceCompare(dy, 0)) {
    owner_->setState(owner_->state() | state_flag_);
  }
  return QTransform::translate(dx, dy);
}

void PhotoTransform::setOwner(PhotoData *owner) { owner_ = owner; }

const PhotoData *PhotoTransform::owner() const { return owner_; }

PhotoData::PhotoData()
    : transform_scale_rotate_(this, (short)STATE::UNCHANGED),
      transform_offset_(this, (short)STATE::UNCHANGED) {}

PhotoData::PhotoData(QPixmap image)
    : transform_scale_rotate_(this, (short)STATE::UNCHANGED),
      transform_offset_(this, (short)STATE::UNCHANGED), image_(image) {}

void PhotoData::resetData(QString image_name, bool clear_state) {
  fillImage(image_name);

  if (clear_state) {
    transform_scale_rotate_.reset();
    transform_offset_.reset();

    state_ |= (short)STATE::CHANGED;
  }
}

void PhotoData::fillImage(QString image_name) { image_id_ = image_name; }

void PhotoData::setTransforms(PhotoTransform transform_scale_rotate,
                              PhotoTransform transform_offset) {

  transform_scale_rotate_ = std::move(transform_scale_rotate);
  transform_offset_ = std::move(transform_offset);

  if (transform_offset_.owner() != this) {
    transform_offset_.setOwner(this);
    state_ |= (short)STATE::CHANGED;
  }

  if (transform_scale_rotate_.owner() != this) {
    transform_scale_rotate_.setOwner(this);
    state_ |= (short)STATE::CHANGED;
  }
}
const QPixmap &PhotoData::image() const {
  return image_.isNull() ? image_manager_->image(image_id_) : image_;
}
const QString PhotoData::imageId() const {
  UNI_ASSERT(!image_id_.isEmpty());
  return image_id_;
}

const PhotoTransform &PhotoData::transformScaleRotate() const {
  return transform_scale_rotate_;
}

const PhotoTransform &PhotoData::transformOffset() const {
  return transform_offset_;
}

PhotoTransform &PhotoData::transformScaleRotateRef() {
  return transform_scale_rotate_;
}

PhotoTransform &PhotoData::transformOffsetRef() { return transform_offset_; }

short PhotoData::state() const { return state_; }

bool PhotoData::isStub() const {
  return image_.isNull() ? image_manager_->isStub(image_id_) : false;
}

void PhotoData::setState(short state) { state_ = state; }

} // namespace FirstYear::Core
