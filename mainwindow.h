#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <Types.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

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
  PhotoTuneWidget *CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(PhotoTuneWidget *photo_tune_widget,
                    Core::FrameControl &frame_control);
  SwipeWidgetsList *CreateSwipeWidget();
  void UpdateFrames(FrameWidgetBase *exept);

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  std::vector<FrameWidgetBase *> widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
