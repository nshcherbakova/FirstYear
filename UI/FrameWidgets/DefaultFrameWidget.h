#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>
#include <QLabel>
#include <QWidget>
#include <Types.h>
#include <UI/Utility.h>

class QPushButton;

namespace FirstYear::UI {

class FrameWidgetBase : public QWidget {
  Q_OBJECT
public:
  explicit FrameWidgetBase(QWidget *parent, Core::FrameControl &control,
                           QString id, std::vector<QRectF> photo_slots,
                           std::vector<FrameParameters> frame_data);
  FrameWidgetBase &operator=(const FrameWidgetBase &) = delete;
  virtual ~FrameWidgetBase(){};

public:
  QString id() const;

signals:
  void SignalTunePhoto(int, FirstYear::UI::FrameParameters,
                       FirstYear::Core::PhotoData);

public slots:
  void Update();

public: // QWidget
  virtual void setVisible(bool visible) override final;

protected:
  virtual void paintEvent(QPaintEvent *e) override final;
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  void load(Core::FrameControl &control);
  void initMonthPhotoWidgets(Core::FrameControl &control);
  void initPhotoTuneWidget(Core::FrameControl &control);
  void createButtons(Core::FrameControl &control);
  void createForegroundWidget();
  void InitPhotos(Core::FrameControl &control);

  QPixmap renderFrame(FirstYear::Core::ProjectPtr);

protected:
  QString id_;
  QPixmap foreground_;
  QPixmap foreground_to_render_;
  std::vector<QRectF> photo_slots_real_;
  std::vector<FrameParameters> frame_data_;

private:
  Core::FrameControl &control_;
  std::vector<PhotoWidget *> photo_widgets_;

  QWidget *foreground_widget_ = nullptr;
  std::vector<QRectF> photo_slots_;

  QPushButton *render_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
};

class DefaultFrameWidget final : public FrameWidgetBase {
  Q_OBJECT
public:
  explicit DefaultFrameWidget(QWidget *parent, Core::FrameControl &control);
  virtual ~DefaultFrameWidget(){};

public:
  DefaultFrameWidget &operator=(const DefaultFrameWidget &) = delete;
};
class DefaultFrameWidget2 final : public FrameWidgetBase {
  Q_OBJECT
public:
  explicit DefaultFrameWidget2(QWidget *parent, Core::FrameControl &control);
  virtual ~DefaultFrameWidget2(){};

public:
  DefaultFrameWidget2 &operator=(const DefaultFrameWidget2 &) = delete;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
