#pragma once
#ifndef FIRSTYEAR_CORE_PROJECT_H
#define FIRSTYEAR_CORE_PROJECT_H
#include <QGuiApplication>
#include <QPixmap>
#include <QRectF>
#include <QScreen>
#include <QString>
#include <Types.h>
#include <vector>

namespace FirstYear::Core {
class PhotoData;
class PhotoTransform : public QTransform {
  friend class PhotoData;

public:
  PhotoTransform(PhotoData *owner, short state_flag);

public:
  void reset();
  QTransform &rotate(qreal a, Qt::Axis axis = Qt::ZAxis);
  QTransform &scale(qreal sx, qreal sy);
  QTransform &translate(qreal dx, qreal dy);

protected:
  void setOwner(PhotoData *owner);
  const PhotoData *owner() const;

private:
  PhotoData *owner_ = nullptr;
  short state_flag_ = 0;
};

class PhotoData {
  friend class FileSystemProjectLoader;
  friend class FileSystemProjectWriter;

public:
  static PhotoData CreateEmptyData();
  static PhotoData CreateStubData(QPixmap image);
  static PhotoData CreateNewData(QPixmap image);
  static PhotoData CreateCopy(const PhotoData &source);

  explicit PhotoData();

public:
  void setImage(QPixmap image);
  /*void setImage(QPixmap image, PhotoTransform transform_scale_rotate,
                const PhotoTransform transform_offset);*/
  void setTransforms(PhotoTransform transform_scale_rotate,
                     PhotoTransform transform_offset);
  void setStubImage(QPixmap image);
  void setState(short state);

  const QPixmap &image() const;
  const PhotoTransform &transformScaleRotate() const;
  const PhotoTransform &transformOffset() const;
  PhotoTransform &transformScaleRotateRef();
  PhotoTransform &transformOffsetRef();
  short state() const;
  bool isStub() const;

  enum class STATE : short {
    IMAGE_CHANGED = 0x01,
    TRANSFORM_SR_CHANGED = 0x02,
    TRANSFORM_OFFSET_CHANGED = 0x04,
  };

protected:
  QPixmap image_;
  bool is_stub_image_ = false;
  PhotoTransform transform_scale_rotate_;
  PhotoTransform transform_offset_;

  mutable short state_ = 0;
};

struct MonthItem {
  PhotoDataPtr photo_data;
  QString text;
  QString filter_id;
  QString stub_image_path;

  mutable short state = 0;
  enum class STATE : short {
    //  PHOTO_DATA_CHANGED = 0x01,
    TEXT_CHANGED = 0x01,
    FILTER_ID_CHANGED = 0x02,
  };
};

class Project {
public:
  using MonthItems = std::vector<MonthItem>;

  QString id_; // project id
  QString title_ = "My First Year";
  QString frame_id_; // frame identificator
  MonthItems monthes_;

  // bool changed = false;
  mutable short state = 0;
  enum class STATE : short {
    ID_CHANGED = 0x01,
    TITLE_CHANGED = 0x02,
    FRAME_ID_CHANGED = 0x04,
  };
};

} // namespace FirstYear::Core
Q_DECLARE_METATYPE(FirstYear::Core::PhotoData);
#endif // FIRSTYEAR_CORE_PROJECT_H
