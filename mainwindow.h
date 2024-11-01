#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <Types.h>

class QPushButton;

namespace FirstYear::UI {
class SwipeWidgetsList;
class PhotoTuneWidget;
class SwipeWidget;
class TemplateWidgetHolder;
class LineEditWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control);
  ~MainWindow();

protected:
private:
  void CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(Core::FrameControl &frame_control);
  void CreateSwipeWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateLineEditWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateButtons(FirstYear::Core::FrameControl &frame_control);
  void UpdateFrames(TemplateWidgetHolder *exept);
  int CurrentTemplateIndex(FirstYear::Core::FrameControl &frame_control) const;

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  // FirstYear::Core::FrameControl &project_control_;
  std::vector<TemplateWidgetHolder *> frame_widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  SwipeWidget *swipe_widget_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  LineEditWidget *line_edit_ = nullptr;

  QPushButton *render_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
