#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Types.h>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(const FirstYear::Core::ProjectPtr &project);
  ~MainWindow();

private:
  // Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
