#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <Types.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

namespace FirstYear::UI {
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control);
  ~MainWindow();

private:
  PhotoTuneWidget *CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(PhotoTuneWidget *photo_tune_widget,
                    Core::FrameControl &frame_control);
  SwipeWidgetsList *CreateSwipeWidget();

private:
  std::vector<FrameWidgetBase *> widgets_;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
