#include "mainwindow.h"
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>
#include <UI/FrameWidgets/PreviewWidget.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>
#include <stdafx.h>

static const int TITLE_ID = -1;
static const char *c_share_image_tmp_name_str = "/ie_tmp.jpg";
static const char *c_save_share_image_format_str = "JPG";
// static const char *c_share_image_extension_str = "jpg";
static const char *c_share_image_mime_type_str = "image/jpeg";

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
    : QMainWindow(), project_control_(frame_control) {

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
  CreatePreviewWindow();

  photo_tune_widget_->raise();

  if (frame_control.CurrentProject()->frame_id_.isEmpty())
    frame_control.CurrentProject()->frame_id_ =
        frame_widgets_[0]->innerWidget()->id();
  int current_fame_index = CurrentTemplateIndex(frame_control);

  swipe_view_->SetCurrentItem(current_fame_index);

  resizeEvent(nullptr);
}

void MainWindow::CreatePreviewWindow() {
  preview_ = new Preview::PreviewWidget(*this);
  preview_->setGeometry({{0, 0}, size()});
  connect(preview_, &Preview::PreviewWidget::SignalShareImage, this,
          [&] { Share(preview_->getImage()); });
  preview_->hide();
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
    SaveTunedImage(month, frame_control);
  });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalOpenFile, this, [&] {
    auto project = frame_control.CurrentProject();

    const int month = photo_tune_widget_->getPhotoId();
    OpenImage(month, frame_control);
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

    connect(widget->innerWidget(),
            &TemplateWidgetBase::SignalRemoveButtonClicked, this,
            [&](int month_index) {
              auto &month_data =
                  frame_control.CurrentProject()->monthes_[month_index];
              month_data.photo_data = {
                  QPixmap(month_data.stub_image_path), true, QTransform(),
                  QTransform(),
                  ((short)PhotoData::STATE::IMAGE_CHANGED |
                   (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
                   (short)PhotoData::STATE::TRANSFORM_SR_CHANGED)};

              month_data.photo_data.image.setDevicePixelRatio(
                  QGuiApplication::primaryScreen()->devicePixelRatio());

              UpdateFrames(nullptr);
            });
  }

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTuneNextImage, this,
          [&]() {
            const auto project = frame_control.CurrentProject();
            int current_month = photo_tune_widget_->getPhotoId();
            int next_month =
                (current_month + 1) % ((int)project->monthes_.size() - 1);

            TuneNewImage(current_month, next_month, frame_control);
          });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTunePrevImage, this,
          [&]() {
            const auto project = frame_control.CurrentProject();
            int current_month = photo_tune_widget_->getPhotoId();
            int next_month =
                (current_month - 1) % ((int)project->monthes_.size() - 1);

            TuneNewImage(current_month, next_month, frame_control);
          });
}

void MainWindow::TuneNewImage(int current_month, int next_month,
                              FirstYear::Core::FrameControl &frame_control) {

  SaveTunedImage(current_month, frame_control);

  auto project = frame_control.CurrentProject();

  TuneImage(next_month, frame_control);
}

bool MainWindow::OpenImage(int month,
                           FirstYear::Core::FrameControl &frame_control) {

  const auto file = Utility::OpenFile(photo_tune_widget_);
  if (!file.isNull()) {
    auto project = frame_control.CurrentProject();
    auto &month_data = project->monthes_[month];
    month_data.photo_data = {
        QPixmap(file), false, QTransform(), QTransform(),
        ((short)PhotoData::STATE::IMAGE_CHANGED |
         (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
         (short)PhotoData::STATE::TRANSFORM_SR_CHANGED)};
    month_data.photo_data.image.setDevicePixelRatio(devicePixelRatio());

    photo_tune_widget_->updatePhoto(month_data.photo_data);
    return true;
  }
  return false;
}

void MainWindow::SaveTunedImage(int month,
                                FirstYear::Core::FrameControl &frame_control) {

  const auto new_photo_data = photo_tune_widget_->getPhoto();
  auto project = frame_control.CurrentProject();
  project->monthes_[month].photo_data = new_photo_data;
  frame_control.SaveProjectMonth(month);
  UpdateFrames(nullptr);
}

void MainWindow::TuneImage(int month,
                           FirstYear::Core::FrameControl &frame_control) {

  auto project = frame_control.CurrentProject();
  auto &month_data = project->monthes_[month];

  if (month_data.photo_data.is_stub_image) {

    if (!OpenImage(month, frame_control)) {
      photo_tune_widget_->hide();
      return;
    }
  }
  const auto frameData =
      frame_widgets_[swipe_view_->CurrentItem()]->innerWidget()->frameData(
          month);
  photo_tune_widget_->setPhoto(month, frameData, month_data.photo_data,
                               month_data.text);
}

void MainWindow::UpdateFrames(TemplateWidgetHolder *exept) {

  for (auto &widget : frame_widgets_) {
    if (exept != widget) {
      widget->innerWidget()->Update();
    }
  }

  UpdateSelectionButton(project_control_);
}

void MainWindow::UpdateSelectionButton(
    FirstYear::Core::FrameControl &frame_control) {
  auto project = frame_control.CurrentProject();
  int empty_slots_count = 0;
  for (const auto &month_data : project->monthes_) {
    if (month_data.photo_data.is_stub_image) {
      empty_slots_count++;
    }
  }
  if (empty_slots_count) {
    select_images_button_->show();
    select_images_button_->setText(
        QString("Select %1 images").arg(empty_slots_count));
  } else
    select_images_button_->hide();
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

  preview_button_ = new QPushButton(this);
  preview_button_->setGeometry(20, height() - 2 * 40, 2 * 40, 40);
  preview_button_->setText("Preview");
  preview_button_->setContentsMargins(0, 0, 0, 0);
  connect(preview_button_, &QPushButton::clicked, this, [&] {
    QPixmap pixmap = Render(control);
    pixmap.setDevicePixelRatio(devicePixelRatio());
#ifdef Q_OS_ANDROID
    // QString path =
    //     QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
    //     + "/test.png";
#else
    QString path ="/Users/nshcherbakova/Desktop/FirstYear/test1.png";
pixmap.save(path);
#endif

    preview_->setImage(pixmap);
    preview_->show();
  });

  share_button_ = new QPushButton(this);
  share_button_->setGeometry(width() - 100, height() - 2 * 40, 2 * 40, 40);
  share_button_->setText("Share");
  share_button_->setContentsMargins(0, 0, 0, 0);
  connect(share_button_, &QPushButton::clicked, this, [&] {
    const QPixmap pixmap = Render(control);
    Share(pixmap);
  });

  select_images_button_ = new QPushButton(this);
  select_images_button_->setGeometry(width() - 100, height() - 2 * 40, 2 * 40,
                                     40);
  select_images_button_->setText(
      QString("Select %1 images")
          .arg(control.CurrentProject()->monthes_.size()));
  select_images_button_->setContentsMargins(0, 0, 0, 0);
  connect(select_images_button_, &QPushButton::clicked, this, [&] {
    const auto image_filse_pathes = Utility::OpenFiles(this);
    std::map<QDateTime, std::vector<QString>> map;
    for (const auto &path : image_filse_pathes) {
      QFileInfo file_info(path);
      QDateTime time;
      if (file_info.birthTime().isValid()) {
        time = file_info.birthTime();
      } else if (file_info.lastModified().isValid()) {
        time = file_info.lastModified();
      } else {
        time = QDateTime::currentDateTime();
      }

      map[time].push_back(path);
    }

    auto project = control.CurrentProject();

    size_t month = 0;
    for (const auto &[_, file_vector] : map) {
      for (const auto &file : file_vector) {
        while (project->monthes_.size() > month) {
          auto &month_data = project->monthes_[month];
          month++;
          if (month_data.photo_data.is_stub_image) {
            month_data.photo_data = {
                QPixmap(file), false, QTransform(), QTransform(),
                ((short)PhotoData::STATE::IMAGE_CHANGED |
                 (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
                 (short)PhotoData::STATE::TRANSFORM_SR_CHANGED)};
            month_data.photo_data.image.setDevicePixelRatio(devicePixelRatio());
            break;
          }
        }
      }
    }
    UpdateFrames(nullptr);
  });

  UpdateSelectionButton(project_control_);
}

void MainWindow::Share(const QPixmap &pixmap) const {
  if (!share_utiles_) {
    share_utiles_ = std::make_shared<ShareUtils::ShareUtilsCpp>();
  }
  auto tmp_path = QStandardPaths::writableLocation(
                      QStandardPaths::StandardLocation::PicturesLocation) +
                  c_share_image_tmp_name_str;

  pixmap.save(tmp_path, c_save_share_image_format_str);

  share_utiles_->sendFile(tmp_path, "View File", c_share_image_mime_type_str,
                          0);
}

QPixmap MainWindow::Render(Core::FrameControl &control) {
  if (control.CurrentProject()->frame_id_ ==
      DefaultTemplateWidget::templateId())
    return DefaultTemplateWidget(this, control, true).renderFrame();
  else if (control.CurrentProject()->frame_id_ ==
           DefaultTemplateWidget2::templateId())
    return DefaultTemplateWidget2(this, control, true).renderFrame();
  return DefaultTemplateWidget(this, control, true).renderFrame();
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

  if (preview_button_)
    preview_button_->setGeometry(20, rect.height() - 2 * 40, 2 * 40, 40);

  if (share_button_)
    share_button_->setGeometry(rect.width() - 100, rect.height() - 2 * 40,
                               2 * 40, 40);

  if (select_images_button_)
    select_images_button_->setGeometry(rect.width() - 100,
                                       rect.height() - 4 * 40, 2 * 40, 40);
  if (preview_)
    preview_->setGeometry(rect);

  update();
}

MainWindow::~MainWindow() {}
} // namespace FirstYear::UI
