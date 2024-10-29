#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>
#include <stdafx.h>

using namespace FirstYear::Core;
namespace FirstYear::UI {
MainWindow::MainWindow(FrameControl &frame_control)
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
  QSize window_size(640, 640);
#elif defined Q_OS_IOS
  const auto screen_size = QApplication::primaryScreen()->geometry().size();
  QSize window_size(screen_size);
#endif
  // setMaximumSize(window_size);
  setMinimumSize(window_size);
  // setFixedSize(window_size);
  setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

  CreatePhotoTuneWidget(frame_control);

  CreateFrames(photo_tune_widget_, frame_control);
  CreateLineEditWidget(frame_control);

  CreateSwipeWidget(frame_control);

  photo_tune_widget_->raise();

  int current_fame_index = 0;
  const auto last_frame = frame_control.CurrentProject()->frame_id_;
  for (int i = 0; i < (int)widgets_.size(); i++) {

    if (widgets_[i]->id() == last_frame) {
      current_fame_index = i;
    }
  }

  swipe_view_->SetCurrentItem(current_fame_index);

  is_initialised = true;
  resizeEvent(nullptr);
}

void MainWindow::CreateLineEditWidget(
    FirstYear::Core::FrameControl &frame_control) {
  line_edit_ = new LineEditWidget(this);
  line_edit_->hide();

  connect(line_edit_, &LineEditWidget::SignalTextChanged, this,
          [&](QString text) {
            auto project = frame_control.CurrentProject();
            if (text == project->title_) {
              return;
            }

            project->title_ = text;
            frame_control.SaveProject();

            UpdateFrames(nullptr);
          });

  for (auto &widget : widgets_) {
    connect(widget, &TemplateWidgetBase::SignalTitleClicked, this,
            [&](QString text) {
              line_edit_->setText(text);
              line_edit_->show();
              line_edit_->raise();
            });
  }
}

void MainWindow::CreatePhotoTuneWidget(
    FirstYear::Core::FrameControl &frame_control) {
  photo_tune_widget_ = new PhotoTuneWidget(*this);
  photo_tune_widget_->hide();

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalImageTuned, this, [&] {
    int month = photo_tune_widget_->getPhotoId();
    const auto new_photo_data = photo_tune_widget_->getPhoto();
    frame_control.CurrentProject()->monthes_[month].photo_data = new_photo_data;
    frame_control.SaveProjectMonth(month);
  });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalOpenFile, this, [&] {
    auto project = frame_control.CurrentProject();

    int month = photo_tune_widget_->getPhotoId();
    auto &month_data = project->monthes_[month];
    const auto file = Utility::OpenFile(photo_tune_widget_);
    if (!file.isNull()) {
      month_data.photo_data = {QPixmap(file), false, QTransform(),
                               QTransform()};
      photo_tune_widget_->updatePhoto(month_data.photo_data);
    }
  });
}
void MainWindow::CreateFrames(PhotoTuneWidget *photo_tune_widget,
                              FirstYear::Core::FrameControl &frame_control) {

  widgets_ = std::vector<TemplateWidgetBase *>{
      new DefaultTemplateWidget(nullptr, frame_control),
      new DefaultTemplateWidget2(nullptr, frame_control)};

  for (auto &widget : widgets_) {
    widget->setGeometry({{0, 0}, size()});
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());

    connect(widget, &TemplateWidgetBase::SignalTunePhoto, this,
            [photo_tune_widget](int month, FrameParameters frame_data,
                                PhotoData photo_data) {
              photo_tune_widget->setPhoto(month, frame_data, photo_data);
              photo_tune_widget->show();
            });

    connect(widget, &TemplateWidgetBase::SignalTextChanged, this,
            [&]() { UpdateFrames(nullptr); });
  }

  connect(photo_tune_widget, &PhotoTuneWidget::SignalImageTuned, this,
          [&]() { UpdateFrames(nullptr); });
}

void MainWindow::UpdateFrames(TemplateWidgetBase *exept) {

  for (auto &widget : widgets_) {
    if (exept != widget) {
      widget->Update();
    }
  }
}

void MainWindow::CreateSwipeWidget(
    FirstYear::Core::FrameControl &frame_control) {
  swipe_view_ = new SwipeWidgetsList(this, widgets_);
  connect(swipe_view_, &SwipeWidgetsList::SignalItemChanged, this,
          [&](int index) {
            frame_control.CurrentProject()->frame_id_ = widgets_[index]->id();
            frame_control.SaveProject();
          });
}

void MainWindow::resizeEvent(QResizeEvent *e) {

  if (!is_initialised) {
    return;
  }

  for (auto &widget : widgets_) {
    //  break;
    widget->setGeometry({{0, 0}, size()});
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());
  }

  if (swipe_view_)
    swipe_view_->setGeometry({{0, 0}, geometry().size()});

  if (photo_tune_widget_)
    photo_tune_widget_->setGeometry({{0, 0}, geometry().size()});

  line_edit_->setGeometry({QPoint(0, 0), size()});
  spdlog::error("MainWindow::resizeEvent w {} h {}", width(), height());
}

MainWindow::~MainWindow() {
  // delete ui;
}
} // namespace FirstYear::UI
