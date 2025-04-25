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
public:
  explicit PhotoData();
  virtual ~PhotoData(){};

public:
  enum class STATE : short {
    UNCHANGED = 0x01,
    CHANGED = 0x01,
  };

  static ImageManagerPtr image_manager_;

public:
  virtual const QPixmap &image() const;
  virtual bool isStub() const;

  void resetData(QString image, bool clear_state);
  void setTransforms(PhotoTransform transform_scale_rotate,
                     PhotoTransform transform_offset);
  void setState(short state);

  const QString imageId() const;
  void setImageId(QString image_id);
  const PhotoTransform &transformScaleRotate() const;
  const PhotoTransform &transformOffset() const;
  PhotoTransform &transformScaleRotateRef();
  PhotoTransform &transformOffsetRef();
  short state() const;

protected:
  QString image_id_;

  PhotoTransform transform_scale_rotate_;
  PhotoTransform transform_offset_;

  mutable short state_ = 0;
};

struct MonthItem {
  PhotoDataPtr photo_data;
  QString text;
  QString filter_id;
  // QString stub_image_path;

  mutable short state = 0;
  enum class STATE : short {
    UNCHANGED = 0x01,
    CHANGED = 0x01,
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
    UNCHANGED = 0x01,
    CHANGED = 0x01,
  };
};

} // namespace FirstYear::Core
Q_DECLARE_METATYPE(FirstYear::Core::PhotoData);
#endif // FIRSTYEAR_CORE_PROJECT_H
