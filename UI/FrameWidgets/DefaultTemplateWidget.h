#pragma once
#ifndef FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#define FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>

#include <Types.h>
#include <UI/Utility.h>

#include <QLabel>
#include <QWidget>

class QPushButton;
class QLineEdit;

namespace FirstYear::UI {

struct TitleParameters {
  QRect title_rect;
  Qt::Alignment alignment;
};

struct PhotoTextParameters {
  std::vector<QPoint> photo_text_anchors;
  Qt::Alignment alignment;
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

class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget *parent, int font_size, QString font_color,
                          QString font_family, bool hide_edit_icon = false);

  void setText(QString text);
  void setFontSize(int size);

  QString text() const;
signals:
  void clicked();

protected:
  void mouseReleaseEvent(QMouseEvent *event);

private:
  QFont font_;
  QString styled_text_;
  QString icon_text_;
  QString text_;
  int icon_size_ = 10;
  bool hide_edit_icon_ = false;
};

class LineEditWidget final : public QWidget {

  Q_OBJECT
public:
  LineEditWidget(QWidget *parent);

public:
  virtual void setVisible(bool visible) override final;
  void setText(QString text, int id);

signals:
  void SignalTextChanged(QString text, int id);

protected:
  virtual void resizeEvent(QResizeEvent *) override final;
  virtual void mouseReleaseEvent(QMouseEvent *) override final;

private:
  QLineEdit *line_edit_ = nullptr;
  int id_ = -1;
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

public: // QWidget
  virtual void setVisible(bool visible) override final;
  FrameParameters frameData(int month);
  QPixmap renderFrame();

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  void load(Core::FrameControl &control);
  void initMonthPhotoWidgets();
  void initPhotoTuneWidget(Core::FrameControl &control);
  void createForegroundWidget();
  void createTitleTextWidget(Qt::Alignment alignment, bool is_rendering);
  void createPhotoTextWidgets(Qt::Alignment alignment, bool is_rendering);
  void createRemoveButtonWidgets(bool is_rendering);
  void InitPhotos(Core::FrameControl &control);

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
