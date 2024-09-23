#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <QLabel>
#include <QWidget>
#include <Types.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>

namespace FirstYear::UI {
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget *parent = Q_NULLPTR,
                          Qt::WindowFlags f = Qt::WindowFlags());
  ~ClickableLabel();

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent *event);
};

class FrameWidgetBase : public QWidget {
  Q_OBJECT
public:
  explicit FrameWidgetBase(QWidget &parent, Core::FrameControl &control,
                           QString id, std::vector<QRectF> photo_slots,
                           std::vector<FrameParameters> frame_data);
  FrameWidgetBase &operator=(const FrameWidgetBase &) = delete;
  virtual ~FrameWidgetBase(){};

public:
  void reload(Core::FrameControl &control);
  QString id() const;

public: // QWidget
  virtual void paintEvent(QPaintEvent *e) override final;
  virtual void setVisible(bool visible) override final;

private:
  void initMonthPhotoWidgets(Core::FrameControl &control);
  void initPhotoTuneWidget(Core::FrameControl &control);
  void createButtons(Core::FrameControl &control);
  void createForegroundWidget();
  void InitPhotos(Core::FrameControl &control);

  QString OpenFile();
  QPixmap renderFrame(FirstYear::Core::ProjectPtr);

protected:
  QString id_;
  QPixmap foreground_;
  QPixmap foreground_to_render_;
  std::vector<QRectF> photo_slots_real_;
  std::vector<FrameParameters> frame_data_;

private:
  std::vector<PhotoWidget *> photo_widgets_;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  ClickableLabel *myLabel_ = nullptr;
  QWidget *foreground_widget_ = nullptr;
  std::vector<QRectF> photo_slots_;
};

class DefaultFrameWidget final : public FrameWidgetBase {
  Q_OBJECT
public:
  explicit DefaultFrameWidget(QWidget &parent, Core::FrameControl &control);
  virtual ~DefaultFrameWidget(){};

public:
  DefaultFrameWidget &operator=(const DefaultFrameWidget &) = delete;
};
class DefaultFrameWidget2 final : public FrameWidgetBase {
  Q_OBJECT
public:
  explicit DefaultFrameWidget2(QWidget &parent, Core::FrameControl &control);
  virtual ~DefaultFrameWidget2(){};

public:
  DefaultFrameWidget2 &operator=(const DefaultFrameWidget2 &) = delete;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
