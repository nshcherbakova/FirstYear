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
      frame_widget->setGeometry(rect());
      frame_widget->setMinimumWidth(width());
      frame_widget->setMinimumHeight(height());
      frame_widget->setMaximumWidth(width());
      frame_widget->setMaximumHeight(height());
    }

    if (swipe_widget_) {
      swipe_widget_->setGeometry(rect());
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

  QSize window_size(9 * 45, 20 * 45);
  setMinimumSize(window_size);
  show();

#endif
  setStyleSheet(c_background_str);

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
  preview_->setGeometry(rect());
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
    OpenImage(month);
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
    widget->setGeometry(rect());
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalImageDroped, this,
            [&](int from_index, int to_index) {
              auto &monthes = frame_control.CurrentProject()->monthes_;

              auto month_from_photo_data = monthes[from_index].photo_data;
              auto &month_to = monthes[to_index];

              monthes[from_index].photo_data = month_to.photo_data;
              month_to.photo_data = month_from_photo_data;

              monthes[from_index].photo_data->setState(
                  ((short)PhotoData::STATE::IMAGE_CHANGED |
                   (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
                   (short)PhotoData::STATE::TRANSFORM_SR_CHANGED));

              monthes[to_index].photo_data->setState(
                  (short)PhotoData::STATE::IMAGE_CHANGED |
                  (short)PhotoData::STATE::TRANSFORM_OFFSET_CHANGED |
                  (short)PhotoData::STATE::TRANSFORM_SR_CHANGED);

              UpdateFrames(nullptr);
              frame_control.SaveProject();
            });

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalTunePhoto, this,
            [&](int month_index) {
              TuneImage(month_index, project_control_);

              // stackedLayout->addWidget(photo_tune_widget_);
            });

    connect(widget->innerWidget(), &TemplateWidgetBase::SignalTextChanged, this,
            [&]() { UpdateFrames(nullptr); });

    connect(widget->innerWidget(),
            &TemplateWidgetBase::SignalRemoveButtonClicked, this,
            [&](int month_index) {
              auto &month_data =
                  frame_control.CurrentProject()->monthes_[month_index];

              month_data.photo_data->setStubImage(
                  QPixmap(month_data.stub_image_path));

              UpdateFrames(nullptr);
            });
  }

  connect(
      photo_tune_widget_, &PhotoTuneWidget::SignalTuneNextImage, this, [&]() {
        const auto project = frame_control.CurrentProject();
        int current_month = photo_tune_widget_->getPhotoId();
        int next_month = (current_month + 1) % ((int)project->monthes_.size());

        TuneNewImage(current_month, next_month, frame_control);
      });

  connect(
      photo_tune_widget_, &PhotoTuneWidget::SignalTunePrevImage, this, [&]() {
        const auto project = frame_control.CurrentProject();
        int current_month = photo_tune_widget_->getPhotoId();
        int next_month = (current_month - 1) % ((int)project->monthes_.size());
        next_month = next_month < 0 ? (int)project->monthes_.size() + next_month
                                    : next_month;

        TuneNewImage(current_month, next_month, frame_control);
      });
}

void MainWindow::TuneNewImage(int current_month, int next_month,
                              FirstYear::Core::FrameControl &frame_control) {

  SaveTunedImage(current_month, frame_control);

  auto project = frame_control.CurrentProject();

  TuneImage(next_month, frame_control);
}

void MainWindow::OpenImage(int month) {
  const auto file = FileDialog::getOpenFileName();
  MainWindow::OnImagePicked(file, month);
}

void MainWindow::OnImagePicked(QString file, int month) {
  auto project = project_control_.CurrentProject();

  auto &month_data = project->monthes_[month];
  if (!file.isNull()) {
    month_data.photo_data->resetData(QPixmap(file));

    TuneImage(month, project_control_);

    project_control_.SaveProjectMonth(month);
  } else if (month_data.photo_data->isStub()) {
    photo_tune_widget_->hide();
  }
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

  if (month_data.photo_data->isStub()) {
    OpenImage(month);
  } else {
    const auto frame_data =
        frame_widgets_[swipe_view_->CurrentItem()]->innerWidget()->frameData(
            month);
    photo_tune_widget_->setPhoto(month, frame_data, month_data.photo_data,
                                 month_data.text);
    photo_tune_widget_->show();
  }
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
    if (month_data.photo_data->isStub()) {
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
  swipe_widget_->setGeometry(rect());

  connect(swipe_view_, &SwipeWidgetsList::SignalItemChanged, this,
          [&](int index) {
            const auto new_id = frame_widgets_[index]->innerWidget()->id();
            if (new_id != frame_control.CurrentProject()->frame_id_) {
              frame_control.CurrentProject()->frame_id_ = new_id;
              frame_control.CurrentProject()->state |=
                  (short)Core::Project::STATE::FRAME_ID_CHANGED;
              frame_control.SaveProject();
            }
          });
}

void MainWindow::CreateButtons(Core::FrameControl &control) {

  preview_button_ = new TextButton(this);
  preview_button_->setStyleSheet(c_white_button_style_str);
  preview_button_->setSize(QSize(120, 60));
  preview_button_->setText("Preview");

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

    preview_->setImage(std::move(pixmap));
    preview_->show();
    // stackedLayout->addWidget(preview_);
  });

  share_button_ = new ShareButton(this);
  connect(share_button_, &QPushButton::clicked, this, [&] {
    const QPixmap pixmap = Render(control);
    Share(pixmap);
  });

  select_images_button_ = new TextButton(this);
  select_images_button_->setStyleSheet(c_select_button_style_str);
  select_images_button_->setText(
      QString("Select %1 images")
          .arg(control.CurrentProject()->monthes_.size()));

  connect(select_images_button_, &QPushButton::clicked, this, [&] {
    const auto files = FileDialog::getOpenFileNames();
    MainWindow::SelectImages(files);
  });

  UpdateSelectionButton(project_control_);
}

void MainWindow::SelectImages(QStringList files) {
  std::map<QString, std::vector<QString>> map;
  for (const auto &path : files) {
    QFileInfo file_info(path);
    if (!file_info.exists()) {
      spdlog::error("SelectImages invalid file {}", path.toStdString());
      continue;
    }

    QString time;
    if (file_info.birthTime().isValid()) {
      time = QString().setNum(file_info.birthTime().toSecsSinceEpoch());
    } else if (file_info.lastModified().isValid()) {
      time = QString().setNum(file_info.lastModified().toSecsSinceEpoch());
    } else {
      time = file_info.fileName();
    }

    map[time].push_back(path);
  }

  auto project = project_control_.CurrentProject();

  size_t month = 0;
  for (const auto &[time, file_vector] : map) {
    //   qDebug() << "**  " <<   time;
    for (const auto &file : file_vector) {
      while (project->monthes_.size() > month) {
        auto &month_data = project->monthes_[month];
        month++;
        if (month_data.photo_data->isStub()) {
          QPixmap image(file);
          if (!image.isNull()) {
            month_data.photo_data->resetData(QPixmap(file));
          } else {
            month--;
            spdlog::error("SelectImages invalid image file {}",
                          file.toStdString());
          }
          break;
        }
      }
    }
  }
  UpdateFrames(nullptr);
  project_control_.SaveProject();
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

  if (swipe_widget_) {
    swipe_widget_->setGeometry(rect());
  }

  if (photo_tune_widget_)
    photo_tune_widget_->setGeometry(rect());

  if (line_edit_)
    line_edit_->setGeometry(rect());

  if (preview_button_) {
    const int diff = share_button_->height() - preview_button_->height();
    preview_button_->setGeometry(
        {{height() / 50,
          height() - height() / 20 - share_button_->height() + diff / 2},
         preview_button_->size()});
  }

  if (share_button_)
    share_button_->setGeometry(
        {{width() - share_button_->width() - height() / 40,
          height() - share_button_->height() - height() / 20},
         share_button_->size()});

  if (select_images_button_)
    select_images_button_->setGeometry(
        {{width() - select_images_button_->width() - height() / 40,
          height() / 20},
         select_images_button_->size()});

  if (preview_)
    preview_->setGeometry(rect());

  update();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
#ifdef Q_OS_MAC
  static bool portrait = true;
  static bool do_small = true;
  static const QSize portret_size = QSize(20 * 45, 9 * 45);
  static const QSize landscape_size = QSize(9 * 45, 20 * 45);

  if (event->key() == Qt::Key_R) {
    QSize window_size;
    if (portrait) {
      window_size = portret_size;
    } else {
      window_size = landscape_size;
    }
    setMinimumSize(window_size);
    setGeometry({geometry().topLeft(), window_size});
    portrait = !portrait;
    do_small = true;
  } else if (event->key() == Qt::Key_S) {
    QSize window_size;
    float scale = do_small ? 0.7 : 1.0 / 0.7;

    window_size = size() * scale;

    setMinimumSize(window_size);
    setGeometry({geometry().topLeft(), window_size});
    do_small = !do_small;
  }
#endif
  QMainWindow::keyReleaseEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {

  if (photo_tune_widget_->isVisible()) {
    // QWidget *top = stackedLayout->currentWidget();
    // stackedLayout->removeWidget( top );
    photo_tune_widget_->hide();
    event->ignore();
  } else if (preview_->isVisible()) {
    preview_->hide();
    event->ignore();
  } else {
    QMainWindow::closeEvent(event);
  }
}

MainWindow::~MainWindow() {}
} // namespace FirstYear::UI
