#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>
#include <QWidget>
#include <Types.h>
#include <UI/Utility.h>

class QPushButton;
class QLineEdit;
class QLabel;

namespace FirstYear::UI {

struct TitleParameters {
  QRect title_rect;
  Qt::Alignment aligment;
};

struct PhotoTextParameters {
  std::vector<QPoint> photo_text_anchors;
  Qt::Alignment aligment;
};

struct TemplateWidgetParameters {
  Core::FrameControl &control;
  QString id;
  TitleParameters title_parameters;
  PhotoTextParameters photo_text_parameters;
  std::vector<QRectF> photo_slots;

  // std::vector<QRectF> photo_text_slots;
  std::vector<FrameParameters> frame_data;
};

class TemplateWidgetBase : public QWidget {
  Q_OBJECT
public:
  explicit TemplateWidgetBase(
      QWidget *parent, const TemplateWidgetParameters &frame_widget_data);
  TemplateWidgetBase &operator=(const TemplateWidgetBase &) = delete;
  virtual ~TemplateWidgetBase(){};

public:
  QString id() const;

signals:
  void SignalTunePhoto(int, FirstYear::UI::FrameParameters,
                       FirstYear::Core::PhotoData);
  void SignalTextChanged();

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
  QRectF title_slot_real_;
  std::vector<QRectF> photo_slots_real_;
  // std::vector<QRectF> photo_text_slots_real_;
  std::vector<QPoint> photo_text_anchors_real_;
  //  Qt::Alignment photo_text_aligment_;
  std::vector<FrameParameters> frame_data_;

private:
  Core::FrameControl &control_;
  std::vector<PhotoWidget *> photo_widgets_;
  std::vector<QLabel *> photo_text_widgets_;

  QWidget *foreground_widget_ = nullptr;
  QRectF title_slot_;
  std::vector<QRectF> photo_slots_;
  // std::vector<QRectF> photo_text_slots_;
  std::vector<QPoint> photo_text_anchors_;

  QPushButton *render_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
  QLineEdit *text_widget_ = nullptr;
};

class DefaultTemplateWidget final : public TemplateWidgetBase {
  Q_OBJECT
public:
  explicit DefaultTemplateWidget(QWidget *parent, Core::FrameControl &control);
  virtual ~DefaultTemplateWidget(){};

public:
  DefaultTemplateWidget &operator=(const DefaultTemplateWidget &) = delete;
};
class DefaultTemplateWidget2 final : public TemplateWidgetBase {
  Q_OBJECT
public:
  explicit DefaultTemplateWidget2(QWidget *parent, Core::FrameControl &control);
  virtual ~DefaultTemplateWidget2(){};

public:
  DefaultTemplateWidget2 &operator=(const DefaultTemplateWidget2 &) = delete;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H