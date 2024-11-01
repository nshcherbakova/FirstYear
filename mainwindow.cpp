#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>
#include <stdafx.h>

static const int TITLE_ID = -1;

using namespace FirstYear::Core;
namespace FirstYear::UI {

class SwipeWidget final : public QWidget {
public:
  SwipeWidget(QWidget *parent) : QWidget(parent) {}
  void addWidgets(std::vector<TemplateWidgetBase *> frame_widgets) {
    frame_widgets_ = frame_widgets;
  }

  void addSwipeView(QWidget *swipe_widget) { swipe_widget_ = swipe_widget; }

protected:
  virtual void resizeEvent(QResizeEvent *event) override final {
    QWidget::resizeEvent(event);

    for (auto &frame_widget : frame_widgets_) {
      //  break;
      frame_widget->setGeometry({{0, 0}, size()});
      frame_widget->setMinimumWidth(width());
      frame_widget->setMinimumHeight(height());
      frame_widget->setMaximumWidth(width());
      frame_widget->setMaximumHeight(height());
    }

    if (swipe_widget_) {
      swipe_widget_->setGeometry({{0, 0}, size()});
    }
  }

private:
  QWidget *swipe_widget_ = nullptr;
  std::vector<TemplateWidgetBase *> frame_widgets_;
};

MainWindow::MainWindow(FrameControl &frame_control)
    : QMainWindow()
//, ui(new Ui::MainWindow)
{

#ifdef Q_OS_ANDROID

  setWindowState(Qt::WindowFullScreen);
  showMaximized();
  setMinimumSize(size());

#elif defined Q_OS_MACOS

  QSize window_size(640, 640);
  setMinimumSize(window_size);
  show();

#endif

  setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

  CreatePhotoTuneWidget(frame_control);

  CreateFrames(frame_control);
  CreateLineEditWidget(frame_control);

  CreateSwipeWidget(frame_control);

  photo_tune_widget_->raise();

  int current_fame_index = 0;
  const auto last_frame = frame_control.CurrentProject()->frame_id_;
  for (int i = 0; i < (int)frame_widgets_.size(); i++) {

    if (frame_widgets_[i]->id() == last_frame) {
      current_fame_index = i;
    }
  }

  swipe_view_->SetCurrentItem(current_fame_index);

  resizeEvent(nullptr);
}

void MainWindow::CreateLineEditWidget(
    FirstYear::Core::FrameControl &frame_control) {
  line_edit_ = new LineEditWidget(this);
  line_edit_->hide();

  connect(line_edit_, &LineEditWidget::SignalTextChanged, this,
          [&](QString text, int id) {
            auto project = frame_control.CurrentProject();

            if (id == TITLE_ID) {
              if (text == project->title_) {
                return;
              }
              project->title_ = text;
            } else {
              UNI_ASSERT(id < (int)project->monthes_.size());
              if (project->monthes_[id].text == text) {
                return;
              }
              project->monthes_[id].text = text;
            }
            frame_control.SaveProject();

            UpdateFrames(nullptr);
            if (photo_tune_widget_->isVisible()) {
              photo_tune_widget_->updateText(text);
            }
          });

  for (auto &widget : frame_widgets_) {
    connect(widget, &TemplateWidgetBase::SignalTitleClicked, this,
            [&](QString text) {
              line_edit_->setText(text, TITLE_ID);
              line_edit_->show();
              line_edit_->raise();
            });

    connect(widget, &TemplateWidgetBase::SignalMonthTextClicked, this,
            [&](QString text, int month) {
              line_edit_->setText(text, month);
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

    UpdateFrames(nullptr);
  });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalOpenFile, this, [&] {
    auto project = frame_control.CurrentProject();

    const int month = photo_tune_widget_->getPhotoId();
    auto &month_data = project->monthes_[month];
    const auto file = Utility::OpenFile(photo_tune_widget_);
    if (!file.isNull()) {
      month_data.photo_data = {QPixmap(file), false, QTransform(),
                               QTransform()};
      photo_tune_widget_->updatePhoto(month_data.photo_data);
    }
  });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTextClicked, this,
          [&](QString text) {
            const int month = photo_tune_widget_->getPhotoId();

            line_edit_->setText(text, month);
            line_edit_->show();
            line_edit_->raise();
          });
}
void MainWindow::CreateFrames(FirstYear::Core::FrameControl &frame_control) {

  frame_widgets_ = std::vector<TemplateWidgetBase *>{
      new DefaultTemplateWidget(nullptr, frame_control),
      new DefaultTemplateWidget2(nullptr, frame_control)};

  for (auto &widget : frame_widgets_) {
    widget->setGeometry({{0, 0}, size()});
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());

    connect(widget, &TemplateWidgetBase::SignalTunePhoto, this,
            [&](int month_index, FrameParameters frame_data) {
              auto &month =
                  frame_control.CurrentProject()->monthes_[month_index];
              photo_tune_widget_->setPhoto(month_index, frame_data,
                                           month.photo_data, month.text);
              photo_tune_widget_->show();
            });

    connect(widget, &TemplateWidgetBase::SignalTextChanged, this,
            [&]() { UpdateFrames(nullptr); });
  }

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTuneNextImage, this,
          [&]() {
            int month = photo_tune_widget_->getPhotoId();
            const auto new_photo_data = photo_tune_widget_->getPhoto();
            auto project = frame_control.CurrentProject();
            project->monthes_[month].photo_data = new_photo_data;
            frame_control.SaveProjectMonth(month);
            UpdateFrames(nullptr);

            if (month == (int)project->monthes_.size() - 1) {
              return;
            }
            month++;
            auto &month_data = project->monthes_[month];

            if (month_data.photo_data.is_stub_image) {
              const auto file = Utility::OpenFile(photo_tune_widget_);
              if (!file.isNull()) {
                month_data.photo_data = {QPixmap(file), false, QTransform(),
                                         QTransform()};
                photo_tune_widget_->updatePhoto(month_data.photo_data);
              } else {
                photo_tune_widget_->hide();
                return;
              }
            }
            const auto frameData =
                frame_widgets_[swipe_view_->CurrentItem()]->frameData(month);
            photo_tune_widget_->setPhoto(
                month, frameData, month_data.photo_data, month_data.text);
          });
}

void MainWindow::UpdateFrames(TemplateWidgetBase *exept) {

  for (auto &widget : frame_widgets_) {
    if (exept != widget) {
      widget->Update();
    }
  }
}

void MainWindow::CreateSwipeWidget(
    FirstYear::Core::FrameControl &frame_control) {
  swipe_widget_ = new SwipeWidget(this);
  swipe_view_ = new SwipeWidgetsList(swipe_widget_, frame_widgets_);
  swipe_widget_->addSwipeView(swipe_view_);
  swipe_widget_->addWidgets(frame_widgets_);
  swipe_widget_->setGeometry({{0, 0}, size()});

  connect(
      swipe_view_, &SwipeWidgetsList::SignalItemChanged, this, [&](int index) {
        frame_control.CurrentProject()->frame_id_ = frame_widgets_[index]->id();
        frame_control.SaveProject();
      });
}

void MainWindow::resizeEvent(QResizeEvent *e) {

  QRect frame_rect;
  const QSize main_widget_size = size();
  // #ifdef Q_OS_ANDROID
  if (!frame_widgets_.empty()) {
    auto frame_widget_size = frame_widgets_[0]->preferedSize(main_widget_size);
    frame_rect = {
        {(main_widget_size.width() - frame_widget_size.width()) / 2,
         (main_widget_size.height() - frame_widget_size.height()) / 2},
        frame_widget_size};
  } else {
    frame_rect = {{0, 0}, geometry().size()};
  }

  if (swipe_widget_) {
    swipe_widget_->setGeometry(frame_rect);
  }

  if (photo_tune_widget_)
    photo_tune_widget_->setGeometry({{0, 0}, size()});

  if (line_edit_)
    line_edit_->setGeometry({{0, 0}, size()});

  if (e) {
    QMainWindow::resizeEvent(e);
  }
  update();
}

MainWindow::~MainWindow() {
  // delete ui;
}
} // namespace FirstYear::UI
