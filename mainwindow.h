#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(FirstYear::Core::FrameControl &frame_control);
  ~MainWindow();

private:
  std::vector<FirstYear::UI::FrameWidgetBase *> widgets_;
};
#endif // MAINWINDOW_H
