#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultFrameWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>

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

  using namespace FirstYear::UI;
  using namespace FirstYear::Core;

  PhotoTuneWidget *photo_tune_widget = new PhotoTuneWidget(*this);
  photo_tune_widget->hide();

  connect(photo_tune_widget, &PhotoTuneWidget::SignalImageTuned, this,
          [photo_tune_widget, &frame_control] {
            int month = photo_tune_widget->getPhotoId();
            const auto new_photo_data = photo_tune_widget->getPhoto();
            frame_control.CurrentProject()->monthes_[month].photo_data =
                new_photo_data;
            frame_control.SaveProjectMonth(month);
          });

  connect(photo_tune_widget, &PhotoTuneWidget::SignalOpenFile, this,
          [photo_tune_widget, &frame_control] {
            auto project = frame_control.CurrentProject();

            int month = photo_tune_widget->getPhotoId()

                ;
            auto &month_data = project->monthes_[month];
            auto file = Utility::OpenFile(photo_tune_widget);
            month_data.photo_data = {QPixmap(file), false, 0, 2.5, QPoint()};
            photo_tune_widget->updatePhoto(month_data.photo_data);
          });

  widgets_ = std::vector<FrameWidgetBase *>{
      new DefaultFrameWidget(nullptr, frame_control),
      new DefaultFrameWidget2(nullptr, frame_control)};

  for (auto &widget : widgets_) {

    widget->setGeometry(geometry());
    widget->setMinimumWidth(geometry().width());
    widget->setMinimumHeight(geometry().height());
    widget->setMaximumWidth(geometry().width());
    widget->setMaximumHeight(geometry().height());

    connect(photo_tune_widget, &PhotoTuneWidget::SignalImageTuned, widget,
            &FrameWidgetBase::Update);

    connect(widget, &FrameWidgetBase::SignalTunePhoto, this,
            [photo_tune_widget](int month, FrameParameters frame_data,
                                PhotoData photo_data) {
              photo_tune_widget->setPhoto(month, frame_data, photo_data);
              photo_tune_widget->show();
            });
  }

  int current_fame_index = 0;
  for (int i = 0; i < (int)widgets_.size(); i++) {

    if (widgets_[i]->id() == frame_control.CurrentProject()->frame_id_) {
      current_fame_index = i;
    }
  }

  SwipeWidgetsList *swipeView = new SwipeWidgetsList(this, widgets_);
  swipeView->setGeometry(geometry());
  swipeView->show();
  swipeView->SetCurrentItem(current_fame_index);
  connect(swipeView, &SwipeWidgetsList::SignalItemChanged, this,
          [&](int index) {
            frame_control.CurrentProject()->frame_id_ = widgets_[index]->id();
            frame_control.SaveProject();
          });
  photo_tune_widget->raise();
}

MainWindow::~MainWindow() {
  // delete ui;
}
