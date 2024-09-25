#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultFrameWidget.h>
#include <stdafx.h>

MainWindow::MainWindow(FirstYear::Core::FrameControl &frame_control)
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

  QSwipeView *swipeView = new QSwipeView(this);
  swipeView->setGeometry(geometry());
  swipeView->addWidget(
      new FirstYear::UI::DefaultFrameWidget(*this, frame_control));
  swipeView->addWidget(
      new FirstYear::UI::DefaultFrameWidget2(*this, frame_control));
  swipeView->show();
}

MainWindow::~MainWindow() {
  // delete ui;
}
