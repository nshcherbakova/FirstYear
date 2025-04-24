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
    : transform_scale_rotate_(this, (short)STATE::TRANSFORM_SR_CHANGED),
      transform_offset_(this, (short)STATE::TRANSFORM_OFFSET_CHANGED) {}

PhotoData::PhotoData(QPixmap image)
    : transform_scale_rotate_(this, (short)STATE::TRANSFORM_SR_CHANGED),
      transform_offset_(this, (short)STATE::TRANSFORM_OFFSET_CHANGED),
      image_(image) {}

PhotoData PhotoData::CreateEmptyData() { return PhotoData(); }

PhotoData PhotoData::CreateNewData(QPixmap image) {
  PhotoData data(std::move(image));
  return data;
}

PhotoData PhotoData::CreateNewData(QString image) {
  PhotoData data;
  data.fillImage(std::move(image));
  return data;
}
PhotoData PhotoData::CreateCopy(const PhotoData &source) { return source; }

void PhotoData::resetData(QString image_name, bool clear_state) {
  fillImage(image_name);

  if (clear_state) {
    transform_scale_rotate_.reset();
    transform_offset_.reset();

    state_ |= (short)STATE::IMAGE_CHANGED;
    state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
    state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
  }
}

void PhotoData::fillImage(QString image_name) {
  // is_stub_image_ = is_stub;
  image_id_ = image_name;
  /*if (scaled) {
    if (image_.width() > image_.height()) {
      image_ =
          image_.scaledToWidth(c_image_scale_size, Qt::SmoothTransformation);
    } else {
      image_ =
          image_.scaledToHeight(c_image_scale_size, Qt::SmoothTransformation);
    }
  }
  image_.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());*/
}
/*
void PhotoData::setImage(QPixmap image, PhotoTransform transform_scale_rotate,
                         const PhotoTransform transform_offset) {
  is_stub_image_ = false;

  image_ = std::move(image);
  image_.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());
  state_ |= (short)STATE::IMAGE_CHANGED;

  setTransforms(std::move(transform_scale_rotate), std::move(transform_offset));
}*/

void PhotoData::setTransforms(PhotoTransform transform_scale_rotate,
                              PhotoTransform transform_offset) {

  transform_scale_rotate_ = std::move(transform_scale_rotate);
  transform_offset_ = std::move(transform_offset);

  if (transform_offset_.owner() != this) {
    transform_offset_.setOwner(this);
    state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
  }

  if (transform_scale_rotate_.owner() != this) {
    transform_scale_rotate_.setOwner(this);
    state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
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
  //  state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
  return transform_scale_rotate_;
}

PhotoTransform &PhotoData::transformOffsetRef() {
  // state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
  return transform_offset_;
}

short PhotoData::state() const { return state_; }

bool PhotoData::isStub() const {
  return image_.isNull() ? image_manager_->isStub(image_id_) : false;
}

void PhotoData::setState(short state) { state_ = state; }

} // namespace FirstYear::Core
