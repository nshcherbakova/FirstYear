#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(FirstYear::Core::FrameControl &frame_control);
  ~MainWindow();

private:
  // Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
