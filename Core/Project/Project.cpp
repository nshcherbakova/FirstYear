#include "Project.h"
#include <stdafx.h>

namespace FirstYear::Core {

void PhotoData::setImage(QPixmap image) {
  is_stub_image_ = false;
  image_ = std::move(image);
  image_.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());
  state_ |= (short)STATE::IMAGE_CHANGED;
}

void PhotoData::setImage(QPixmap image, QTransform transform_scale_rotate,
                         const QTransform transform_offset) {
  is_stub_image_ = false;

  image_ = std::move(image);
  image_.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());
  state_ |= (short)STATE::IMAGE_CHANGED;

  setTransforms(std::move(transform_scale_rotate), std::move(transform_offset));
}
void PhotoData::setTransforms(QTransform transform_scale_rotate,
                              QTransform transform_offset) {

  transform_scale_rotate_ = std::move(transform_scale_rotate);
  transform_offset_ = std::move(transform_offset);

  state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
  state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
}
void PhotoData::setStubImage(QPixmap image) {
  image_ = std::move(image);
  image_.setDevicePixelRatio(
      QGuiApplication::primaryScreen()->devicePixelRatio());
  is_stub_image_ = true;

  transform_scale_rotate_.reset();
  transform_offset_.reset();

  state_ |= (short)STATE::IMAGE_CHANGED;
  state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
  state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
}
const QPixmap &PhotoData::image() const { return image_; }

const QTransform &PhotoData::transformScaleRotate() const {
  return transform_scale_rotate_;
}

const QTransform &PhotoData::transformOffset() const {
  return transform_offset_;
}

QTransform &PhotoData::transformScaleRotateRef() {
  state_ |= (short)STATE::TRANSFORM_SR_CHANGED;
  return transform_scale_rotate_;
}

QTransform &PhotoData::transformOffsetRef() {
  state_ |= (short)STATE::TRANSFORM_OFFSET_CHANGED;
  return transform_offset_;
}

short PhotoData::state() const { return state_; }

bool PhotoData::isStub() const { return is_stub_image_; }

void PhotoData::setState(short state) { state_ = state; }

} // namespace FirstYear::Core
