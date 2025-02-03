#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>

#include <Types.h>
#include <UI/Utility.h>

#include <QWidget>

class QPushButton;

namespace FirstYear::UI {

class ClickableLabel;

struct TextParameters {
  Qt::Alignment alignment;
  QString font;
  double font_size = 0;
  QString font_color;
};

struct TitleParameters {
  QRect title_rect;
  TextParameters text_parameters;
};

struct PhotoTextParameters {
  QPoint text_anchor;
  TextParameters text_parameters;
};

struct MonthParameters {
  PhotoTextParameters text_parameters;
  QRectF photo_slot;
  FrameParameters frame_data;
};

struct PhotoFrameParameters {
  TitleParameters title_parameters;
  std::vector<MonthParameters> months_parameters;
};

struct TemplateWidgetParameters {
  Core::FrameControl &control;
  QString id;
};

class ForegroundWidget;

class TemplateWidgetBase : public QWidget {
  Q_OBJECT
public:
  explicit TemplateWidgetBase(QWidget *parent,
                              const TemplateWidgetParameters &frame_widget_data,
                              bool render_state = false);
  TemplateWidgetBase &operator=(const TemplateWidgetBase &) = delete;
  virtual ~TemplateWidgetBase(){};

signals:
  void SignalTunePhoto(int /*, FirstYear::UI::FrameParameters*/);
  void SignalTextChanged();
  void SignalTitleClicked(QString text);
  void SignalMonthTextClicked(QString text, int month);
  void SignalRemoveButtonClicked(int month);
  void SignalImageDroped(int from_id, int to_id);

public slots:
  void Update();

public:
  QString id() const;
  QSize preferedSize(QSize size) const;
  FrameParameters frameData(int month);
  QPixmap renderFrame();

public: // QWidget
  virtual void setVisible(bool visible) override final;

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  void load(Core::FrameControl &control);
  void initMonthPhotoWidgets();
  void initPhotoTuneWidget(Core::FrameControl &control);
  void createForegroundWidget();
  void createTitleTextWidget(const TextParameters &parameters,
                             bool is_rendering);
  void createPhotoTextWidgets(const TextParameters &parameters,
                              bool is_rendering);
  void createRemoveButtonWidgets(bool is_rendering);
  void InitPhotos(Core::FrameControl &control);

  void fillParameters(const PhotoFrameParameters &parameters);

protected:
  QString id_;
  QPixmap foreground_;
  QRectF title_slot_real_;
  int title_text_font_size_real_ = 20;
  std::vector<QRectF> photo_slots_real_;
  std::vector<QPoint> photo_text_anchors_real_;
  int photo_text_font_size_real_ = 20;
  std::vector<FrameParameters> frame_data_;

private:
  Core::FrameControl &control_;
  std::vector<PhotoWidget *> photo_widgets_;
  std::vector<ClickableLabel *> photo_text_widgets_;
  std::vector<QPushButton *> remove_buttons_;

  ForegroundWidget *foreground_widget_ = nullptr;
  QRectF title_slot_;
  int title_text_font_size_ = 20;
  std::vector<QRectF> photo_slots_;

  std::vector<QPoint> photo_text_anchors_;
  int photo_text_font_size_ = 20;

  ClickableLabel *title_text_widget_ = nullptr;
  bool render_state_ = false;
};

class DefaultTemplateWidget final : public TemplateWidgetBase {
  Q_OBJECT
public:
  explicit DefaultTemplateWidget(QWidget *parent, Core::FrameControl &control,
                                 bool render_state = false);
  virtual ~DefaultTemplateWidget(){};

  static QString templateId();

public:
  DefaultTemplateWidget &operator=(const DefaultTemplateWidget &) = delete;
};

class DefaultTemplateWidget2 final : public TemplateWidgetBase {
  Q_OBJECT
public:
  explicit DefaultTemplateWidget2(QWidget *parent, Core::FrameControl &control,
                                  bool render_state = false);
  virtual ~DefaultTemplateWidget2(){};

  static QString templateId();

public:
  DefaultTemplateWidget2 &operator=(const DefaultTemplateWidget2 &) = delete;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
