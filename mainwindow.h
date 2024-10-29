#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <Types.h>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>

namespace FirstYear::UI {
class SwipeWidgetsList;
class PhotoTuneWidget;
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control);
  ~MainWindow();

protected:
private:
  void CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(PhotoTuneWidget *photo_tune_widget,
                    Core::FrameControl &frame_control);
  void CreateSwipeWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateLineEditWidget(FirstYear::Core::FrameControl &frame_control);
  void UpdateFrames(TemplateWidgetBase *exept);

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  std::vector<TemplateWidgetBase *> widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  LineEditWidget *line_edit_ = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
