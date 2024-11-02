#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>
#include <stdafx.h>

static const int TITLE_ID = -1;

using namespace FirstYear::Core;
namespace FirstYear::UI {

class TemplateWidgetHolder final : public QWidget {
public:
  TemplateWidgetHolder(QWidget *parent, TemplateWidgetBase *widget)
      : QWidget(parent), widget_(widget) {
    widget_->setParent(this);
  }

  TemplateWidgetBase *innerWidget() { return widget_; }

protected:
  virtual void resizeEvent(QResizeEvent *event) override final {
    QWidget::resizeEvent(event);

    const QSize holder_widget_size = size();

    const auto frame_widget_size = widget_->preferedSize(holder_widget_size);
    QRect frame_rect = {
        {(holder_widget_size.width() - frame_widget_size.width()) / 2,
         (holder_widget_size.height() - frame_widget_size.height()) / 2},
        frame_widget_size};

    widget_->setGeometry(frame_rect);
  }

private:
  TemplateWidgetBase *widget_ = nullptr;
};

class SwipeWidget final : public QWidget {
public:
  SwipeWidget(QWidget *parent) : QWidget(parent) {}
  void addWidgets(std::vector<TemplateWidgetHolder *> frame_widgets) {
    frame_widgets_ = frame_widgets;
  }

  void addSwipeView(QWidget *swipe_widget) { swipe_widget_ = swipe_widget; }

protected:
  virtual void resizeEvent(QResizeEvent *event) override final {
    QWidget::resizeEvent(event);

    for (auto &frame_widget : frame_widgets_) {
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
  std::vector<TemplateWidgetHolder *> frame_widgets_;
};

MainWindow::MainWindow(FrameControl &frame_control)
    : QMainWindow() //, project_control_(frame_control)
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
  CreateButtons(frame_control);

  photo_tune_widget_->raise();

  int current_fame_index = CurrentTemplateIndex(frame_control);

  swipe_view_->SetCurrentItem(current_fame_index);

  resizeEvent(nullptr);
}

int MainWindow::CurrentTemplateIndex(
    FirstYear::Core::FrameControl &frame_control) const {
  int current_fame_index = 0;
  const auto last_frame = frame_control.CurrentProject()->frame_id_;
  for (int i = 0; i < (int)frame_widgets_.size(); i++) {

    if (frame_widgets_[i]->innerWidget()->id() == last_frame) {
      current_fame_index = i;
      break;
    }
  }
  return current_fame_index;
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
              project->state |= (short)Core::Project::STATE::TITLE_CHANGED;
            } else {
              UNI_ASSERT(id < (int)project->monthes_.size());
              if (project->monthes_[id].text == text) {
                return;
              }
              project->monthes_[id].text = text;
              project->monthes_[id].state |=
                  (short)Core::MonthItem::STATE::TEXT_CHANGED;
            }
            frame_control.SaveProject();

            UpdateFrames(nullptr);
            if (photo_tune_widget_->isVisible()) {
              photo_tune_widget_->updateText(text);
            }
          });

  for (auto &widget : frame_widgets_) {
    connect(widget->innerWidget(), &TemplateWidgetBase::SignalTitleClicked,
            this, [&](QString text) {
              line_edit_->setText(text, TITLE_ID);
              line_edit_->show();
              line_edit_->raise();
            });

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalMonthTextClicked,
            this, [&](QString text, int month) {
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
      month_data.photo_data = {
          QPixmap(file), false, QTransform(), QTransform(),
          ((short)PhotoData::STATE::IMAGE_CHANGED |
           (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
           (short)PhotoData::STATE::TRANSFORM_SR_CHANGED)};
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

  frame_widgets_ = std::vector<TemplateWidgetHolder *>{
      new TemplateWidgetHolder(
          nullptr, new DefaultTemplateWidget(nullptr, frame_control)),
      new TemplateWidgetHolder(
          nullptr, new DefaultTemplateWidget2(nullptr, frame_control))};

  for (auto &widget : frame_widgets_) {
    widget->setGeometry({{0, 0}, size()});
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalTunePhoto, this,
            [&](int month_index, FrameParameters frame_data) {
              auto &month =
                  frame_control.CurrentProject()->monthes_[month_index];
              photo_tune_widget_->setPhoto(month_index, frame_data,
                                           month.photo_data, month.text);
              photo_tune_widget_->show();
            });

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalTextChanged, this,
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
                month_data.photo_data = {
                    QPixmap(file), false, QTransform(), QTransform(),
                    ((short)PhotoData::STATE::IMAGE_CHANGED |
                     (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
                     (short)PhotoData::STATE::TRANSFORM_SR_CHANGED)};
                photo_tune_widget_->updatePhoto(month_data.photo_data);
              } else {
                photo_tune_widget_->hide();
                return;
              }
            }
            const auto frameData = frame_widgets_[swipe_view_->CurrentItem()]
                                       ->innerWidget()
                                       ->frameData(month);
            photo_tune_widget_->setPhoto(
                month, frameData, month_data.photo_data, month_data.text);
          });
}

void MainWindow::UpdateFrames(TemplateWidgetHolder *exept) {

  for (auto &widget : frame_widgets_) {
    if (exept != widget) {
      widget->innerWidget()->Update();
    }
  }
}

void MainWindow::CreateSwipeWidget(
    FirstYear::Core::FrameControl &frame_control) {
  swipe_widget_ = new SwipeWidget(this);
  swipe_view_ = new SwipeWidgetsList(
      swipe_widget_, {frame_widgets_.begin(), frame_widgets_.end()});
  swipe_widget_->addSwipeView(swipe_view_);
  swipe_widget_->addWidgets(frame_widgets_);
  swipe_widget_->setGeometry({{0, 0}, size()});

  connect(swipe_view_, &SwipeWidgetsList::SignalItemChanged, this,
          [&](int index) {
            frame_control.CurrentProject()->frame_id_ =
                frame_widgets_[index]->innerWidget()->id();
            frame_control.CurrentProject()->state |=
                (short)Core::Project::STATE::FRAME_ID_CHANGED;
            frame_control.SaveProject();
          });
}

void MainWindow::CreateButtons(Core::FrameControl &control) {

  render_button_ = new QPushButton(this);
  render_button_->setGeometry(20, height() - 2 * 40, 2 * 40, 40);
  render_button_->setText("Render");
  render_button_->setContentsMargins(0, 0, 0, 0);
  connect(render_button_, &QPushButton::clicked, this, [&] {
    QPixmap pixmap;
    if (control.CurrentProject()->frame_id_ ==
        DefaultTemplateWidget::templateId())
      pixmap = DefaultTemplateWidget(this, control, true).renderFrame();
    else if (control.CurrentProject()->frame_id_ ==
             DefaultTemplateWidget2::templateId())
      pixmap = DefaultTemplateWidget2(this, control, true).renderFrame();

#ifdef Q_OS_ANDROID
      // QString path =
      //     QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
      //     + "/test.png";
#else
    QString path ="/Users/nshcherbakova/Desktop/FirstYear/test1.png";
pixmap.save(path);
#endif

    QLabel *l = new QLabel("test", this);
    l->setGeometry({{0, 0}, size()});
    l->setPixmap(pixmap);
    l->show();
  });

  share_button_ = new QPushButton(this);
  share_button_->setGeometry(width() - 100, height() - 2 * 40, 2 * 40, 40);
  share_button_->setText("Share");
  share_button_->setContentsMargins(0, 0, 0, 0);
  connect(share_button_, &QPushButton::clicked, this, [&] {

  });
}

void MainWindow::resizeEvent(QResizeEvent *e) {
  if (e) {
    QMainWindow::resizeEvent(e);
  }

  const QRect rect = {{0, 0}, size()};

  if (swipe_widget_) {
    swipe_widget_->setGeometry(rect);
  }

  if (photo_tune_widget_)
    photo_tune_widget_->setGeometry(rect);

  if (line_edit_)
    line_edit_->setGeometry(rect);

  if (render_button_)
    render_button_->setGeometry(20, rect.height() - 2 * 40, 2 * 40, 40);
  if (share_button_)
    share_button_->setGeometry(rect.width() - 100, rect.height() - 2 * 40,
                               2 * 40, 40);

  update();
}

MainWindow::~MainWindow() {}
} // namespace FirstYear::UI
