#include "mainwindow.h"
#include <stdafx.h>

MainWindow::MainWindow(const FirstYear::Core::ProjectPtr &project)
    : QMainWindow()
//, ui(new Ui::MainWindow)
{
#ifdef Q_OS_ANDROID
  setWindowState(Qt::WindowFullScreen);
  showMaximized();
  QSize window_size(size());
#elif WIN32
  QSize window_size(640, 900);
  // QSize window_size(640, 1136);
#elif defined Q_OS_MACOS
  QSize window_size(640, 900);
#elif defined Q_OS_IOS
  const auto screen_size = QApplication::primaryScreen()->geometry().size();
  QSize window_size(screen_size);
#endif
  setMaximumSize(window_size);
  setMinimumSize(window_size);
  setFixedSize(window_size);
  setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

  // todo list of farmes and registration system
  auto frame_widget = (new FirstYear::UI::DefaultFrameWidget(*this, project));
  frame_widget->setEnabled(true);
  frame_widget->setVisible(true);
  frame_widget->show();
  // ui->setupUi(this);
}

MainWindow::~MainWindow() {
  // delete ui;
}
