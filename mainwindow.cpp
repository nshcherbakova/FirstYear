#include "mainwindow.h"
#include <QProgressDialog>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/InfoWidget.h>
#include <UI/FrameWidgets/PhotoTuneWidget.h>
#include <UI/FrameWidgets/PreviewWidget.h>
#include <UI/FrameWidgets/RearrangeWidget.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>

#include <stdafx.h>

static const int TITLE_ID = -1;
static const char *c_share_image_tmp_name_str = "/FirstYear.jpg";
static const char *c_save_share_image_format_str = "JPG";
static const char *c_share_image_mime_type_str = "image/jpeg";
static const char *c_share_text_mime_type_str = "text/plain";

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

static const char *c_swipe_widget_style_str = "QWidget{"
                                              "background: transparent;"
                                              "border: none;"
                                              "}";

class SwipeForeground : public QWidget {

public:
  SwipeForeground(QWidget *parent)
      : QWidget(parent)

  {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    left_swipe_arrow_ = new QSvgRenderer(this);
    left_swipe_arrow_->load(QString(":/images/icons/left"));

    right_swipe_arrow_ = new QSvgRenderer(this);
    right_swipe_arrow_->load(QString(":/images/icons/right"));
  }
  virtual void paintEvent(QPaintEvent *event) override final {
    QWidget::paintEvent(event);
    QPainter painter(this);
    const int iheight = 100;
    if (left_swipe_arrow_ && right_swipe_arrow_) {
      if (left_swipe_arrow_visible_) {
        left_swipe_arrow_->render(
            &painter, QRectF(-5, height() / 2 - iheight / 2, 50, iheight));
      }
      if (right_swipe_arrow_visible_) {
        right_swipe_arrow_->render(
            &painter,
            QRect(width() - 45, height() / 2 - iheight / 2, 50, iheight));
      }
    }
  }

  bool left_swipe_arrow_visible_ = false;
  bool right_swipe_arrow_visible_ = false;

private:
  QSvgRenderer *left_swipe_arrow_ = nullptr;
  QSvgRenderer *right_swipe_arrow_ = nullptr;
};

class SwipeWidget final : public QWidget {
public:
  SwipeWidget(QWidget *parent) : QWidget(parent) {
    setStyleSheet(c_swipe_widget_style_str);
    foreground_ = new SwipeForeground(this);
  }

  void addSwipeView(SwipeWidgetsList *swipe_widget) {
    swipe_widget_ = swipe_widget;

    foreground_->raise();
    /* connect(QScroller::scroller(swipe_widget_), &QScroller::stateChanged,
       this,
             [&](QScroller::State newState) {
               if (newState == QScroller::Inactive) {

                 foreground_->left_swipe_arrow_visible_ = pressed_;
                 foreground_->right_swipe_arrow_visible_ = pressed_;
                 foreground_->update();

               } else {

                 foreground_->left_swipe_arrow_visible_ =
                     (swipe_widget_->CurrentItem() != 0);
                 foreground_->right_swipe_arrow_visible_ =
                     (swipe_widget_->CurrentItem() !=
                      (int)swipe_widget_->Count() - 1);
               }
             });*/

    foreground_->left_swipe_arrow_visible_ =
        (swipe_widget_->CurrentItem() != 0);
    foreground_->right_swipe_arrow_visible_ =
        (swipe_widget_->CurrentItem() != (int)swipe_widget_->Count() - 1);

    connect(swipe_widget_, &SwipeWidgetsList::SignalItemChanged, this,
            [&](int index) {
              foreground_->left_swipe_arrow_visible_ = (index != 0);
              foreground_->right_swipe_arrow_visible_ =
                  (swipe_widget_->CurrentItem() != swipe_widget_->Count() - 1);
            });
  }

protected:
  virtual void resizeEvent(QResizeEvent *event) override final {
    QWidget::resizeEvent(event);

    foreground_->setGeometry(rect());

    if (swipe_widget_) {
      swipe_widget_->setGeometry(rect());
    }
  }

  /* virtual void mousePressEvent(QMouseEvent *event) override final {
     QWidget::mousePressEvent(event);
     pressed_ = true;
     foreground_->left_swipe_arrow_visible_ =
         (swipe_widget_->CurrentItem() != 0);
     foreground_->right_swipe_arrow_visible_ =
         (swipe_widget_->CurrentItem() != (int)swipe_widget_->Count() - 1);
     foreground_->update();
   }*/

  /* virtual void mouseReleaseEvent(QMouseEvent *event) override final {
     QWidget::mouseReleaseEvent(event);
     pressed_ = false;

     if (QScroller::scroller(swipe_widget_)->state() == QScroller::Inactive) {
       foreground_->left_swipe_arrow_visible_ = false;
       foreground_->right_swipe_arrow_visible_ = false;
     }
     foreground_->update();
   }*/

private:
  SwipeWidgetsList *swipe_widget_ = nullptr;

  SwipeForeground *foreground_ = nullptr;
  // bool pressed_ = false;
};

MainWindow::MainWindow(FrameControl &frame_control, const QStringList &frames)
    : QMainWindow(), project_control_(frame_control) {

#ifdef Q_OS_ANDROID

  setWindowState(Qt::WindowFullScreen);
  showMaximized();
  setMinimumSize(size());

#elif defined Q_OS_MACOS

  QSize window_size(9 * 45, 20 * 45);
  //    QSize window_size(16 * 80, 9 * 80);
  setMinimumSize(window_size);
  show();
#else
  QSize window_size(316, 646);
  //  setMinimumSize(window_size);
  show();
#endif

  setStyleSheet(c_background_str);

  setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

  background_svg_ = new QSvgWidget(":/images/icons/stars", this);
  controls_.push_back(background_svg_);

  CreatePhotoTuneWidget(frame_control);
  CreateFrames(frame_control, frames);
  CreateLineEditWidget(frame_control);
  CreateSwipeWidget(frame_control);
  CreateButtons(frame_control);
  CreateTapText();
  CreatePreviewWindow();
  CreateRearrangeWidget(frame_control);
  CreateInfoWidget();

  photo_tune_widget_->raise();

  if (frame_control.CurrentProject()->frame_id_.isEmpty()) {
    frame_control.CurrentProject()->frame_id_ =
        frame_widgets_[0]->innerWidget()->id();
  }
  int current_fame_index = CurrentTemplateIndex(frame_control);

  swipe_view_->SetCurrentItem(current_fame_index);

  resizeEvent(nullptr);
}

void MainWindow::CreatePreviewWindow() {
  preview_ = new Preview::PreviewWidget(*this);
  preview_->setGeometry(rect());
  preview_->hide();
  connect(preview_, &Preview::PreviewWidget::SignalShareImage, this, [&] {
    auto f = [&]() {
      QMetaObject::invokeMethod(
          this, [&]() { Share(preview_->getImage()); }, Qt::QueuedConnection);
    };
    ShowLoadingDialogStub(f);
  });
  connect(preview_, &Preview::PreviewWidget::SignalClosed, this,
          [&] { setEnabledControls(true); });

  child_widgets_.push_back(preview_);
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
              project->state |= (short)Core::Project::STATE::CHANGED;
            } else {
              UNI_ASSERT(id < (int)project->monthes_.size());
              if (project->monthes_[id].text == text) {
                return;
              }
              project->monthes_[id].text = text;
              project->monthes_[id].state |=
                  (short)Core::MonthItem::STATE::CHANGED;
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
    setEnabledControls(true);
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

  child_widgets_.push_back(photo_tune_widget_);
}

void MainWindow::CreateInfoWidget() {
  info_widget_ = new InfoWidget(this, project_control_);
  info_widget_->hide();

  connect(
      info_widget_, &InfoWidget::SignalShareLog, this, [&]() { ShareLog(); },
      Qt::QueuedConnection);

  child_widgets_.push_back(info_widget_);
}

void MainWindow::CreateRearrangeWidget(
    FirstYear::Core::FrameControl &frame_control) {
  rearrange_ = new RearrangeWidget(this, frame_control);
  rearrange_->hide();

  connect(
      rearrange_, &RearrangeWidget::SignalImageDroped, this,
      [&](int from_index, int to_index) {
        auto &monthes = frame_control.CurrentProject()->monthes_;

        auto &month_to = monthes[to_index];
        auto month_from_photo_data = monthes[from_index].photo_data;

        if (!month_to.photo_data->isStub()) {
          int index = from_index > to_index ? (to_index + 1) : (to_index - 1);
          PhotoDataPtr tmp_photodata = month_to.photo_data;
          while (true) {
            auto &cur_month = monthes[index];

            auto new_tmp_photodata = cur_month.photo_data;
            cur_month.photo_data = tmp_photodata;
            tmp_photodata = new_tmp_photodata;

            monthes[index].photo_data->setState(
                (short)PhotoData::STATE::CHANGED);

            if (index == from_index || tmp_photodata->isStub()) {
              break;
            }

            index = from_index > to_index ? (index + 1) : (index - 1);
          }

          if (tmp_photodata->isStub()) {
            monthes[from_index].photo_data = tmp_photodata;
          }
        } else {
          monthes[from_index].photo_data = month_to.photo_data;
        }

        month_to.photo_data = month_from_photo_data;

        monthes[from_index].photo_data->setState(
            ((short)PhotoData::STATE::CHANGED));

        monthes[to_index].photo_data->setState(
            ((short)PhotoData::STATE::CHANGED));

        rearrange_->Update();
      },
      Qt::QueuedConnection);

  connect(rearrange_, &RearrangeWidget::SignalHide, this, [&]() {
    UpdateFrames(nullptr);
    frame_control.SaveProject();
  });

  connect(
      rearrange_, &RearrangeWidget::SignalDeleteButtonClicked, this,
      [&](std::vector<int> month_indexes) { DeletePhotos(month_indexes); },
      Qt::QueuedConnection);

  child_widgets_.push_back(rearrange_);
}

void MainWindow::CreateFrames(FirstYear::Core::FrameControl &frame_control,
                              const QStringList &frames) {

  auto widgets = FrameWidgetsFactory::createWidgets(frames, frame_control);
  // std::sort(widgets.begin(), widgets.end(),
  //           [](const TemplateWidgetBase *v1, const TemplateWidgetBase *v2) {
  //             return v1->position() < v2->position();
  //           });

  for (const auto &widget : widgets) {
    frame_widgets_.push_back(new TemplateWidgetHolder(nullptr, widget));
  }

  for (auto &widget : frame_widgets_) {
    widget->setGeometry(rect());
    widget->setMinimumWidth(width());
    widget->setMinimumHeight(height());
    widget->setMaximumWidth(width());
    widget->setMaximumHeight(height());

    connect(
        widget->innerWidget(), &TemplateWidgetBase::SignalTunePhoto, this,
        [&](int month_index) { TuneImage(month_index, project_control_); },
        Qt::QueuedConnection);

    connect(
        widget->innerWidget(), &TemplateWidgetBase::SignalTextChanged, this,
        [&]() { UpdateFrames(nullptr); }, Qt::QueuedConnection);
  }

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTuneNextImage, this,
          [&]() {
            const auto project = frame_control.CurrentProject();
            int current_month = photo_tune_widget_->getPhotoId();
            int next_month = current_month;
            do {
              next_month = (next_month + 1) % ((int)project->monthes_.size());
            } while (project->monthes_[next_month].photo_data->isStub());

            TuneNewImage(current_month, next_month, frame_control);
          });

  connect(photo_tune_widget_, &PhotoTuneWidget::SignalTunePrevImage, this,
          [&]() {
            const auto project = frame_control.CurrentProject();
            int current_month = photo_tune_widget_->getPhotoId();
            int next_month = current_month;
            do {
              next_month = next_month == 0 ? (int)project->monthes_.size() - 1
                                           : next_month - 1;
            } while (project->monthes_[next_month].photo_data->isStub());

            TuneNewImage(current_month, next_month, frame_control);
          });
}

void MainWindow::DeletePhoto(int month_index) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Delete this photo?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  int ret = msgBox.exec();

  if (QMessageBox::Yes == ret) {
    auto &month_data = project_control_.CurrentProject()->monthes_[month_index];

    month_data.photo_data->resetData(QString::number(month_index), true);
    project_control_.imageManager()->deleteImage(
        month_data.photo_data->imageId());

    UpdateFrames(nullptr);
  }
}

void MainWindow::DeletePhotos(std::vector<int> month_indexes) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(month_indexes.size() == 1 ? tr("Delete this photo?")
                                                  : tr("Delete this photos?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  int ret = msgBox.exec();

  if (QMessageBox::Yes == ret) {
    for (const auto &month_index : month_indexes) {
      auto &month_data =
          project_control_.CurrentProject()->monthes_[month_index];

      project_control_.imageManager()->deleteImage(
          month_data.photo_data->imageId());
      month_data.photo_data->resetData(QString::number(month_index), true);
    }
    UpdateFrames(nullptr);
  }
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

    const auto image_name = project_control_.imageManager()->addImage(file);
    if (image_name.isNull()) {
      QMessageBox msgBox;
      msgBox.setWindowTitle("Can't open image");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();
      return;
    }

    if (!month_data.photo_data->isStub()) {
      project_control_.imageManager()->deleteImage(
          month_data.photo_data->imageId());
    }
    month_data.photo_data->resetData(image_name, true);

    TuneImage(month, project_control_);

    project_control_.SaveProjectMonth(month);
  } else if (month_data.photo_data->isStub() &&
             photo_tune_widget_->isVisible()) {
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

    setEnabledControls(false);
  }
}

void MainWindow::UpdateFrames(TemplateWidgetHolder *exept) {

  for (auto &widget : frame_widgets_) {
    if (exept != widget) {
      widget->innerWidget()->Update();
    }
  }
  rearrange_->Update();

  UpdateSelectionButton(project_control_);
  UpdateRearrangeButton();
  UpdateTapText();
  resizeEvent(nullptr);
}

int MainWindow::LoadedPhotosCount(
    FirstYear::Core::FrameControl &frame_control) const {
  auto project = frame_control.CurrentProject();
  int count = 0;
  for (const auto &month_data : project->monthes_) {
    if (!month_data.photo_data->isStub()) {
      count++;
    }
  }
  return count;
}

void MainWindow::UpdateSelectionButton(
    FirstYear::Core::FrameControl &frame_control) {
  auto project = frame_control.CurrentProject();
  const int empty_slots_count =
      project->monthes_.size() - LoadedPhotosCount(frame_control);

  select_images_button_->setEnabled(empty_slots_count > 0);
  select_images_button_->setText(SelectButtonText(empty_slots_count));
}

void MainWindow::UpdateRearrangeButton() {
  rearrange_button_->setVisible(LoadedPhotosCount(project_control_) > 0);
}

void MainWindow::UpdateTapText() {
  tap_text_->setVisible(LoadedPhotosCount(project_control_) > 0);
}

void MainWindow::CreateSwipeWidget(
    FirstYear::Core::FrameControl &frame_control) {
  swipe_widget_ = new SwipeWidget(this);
  swipe_view_ = new SwipeWidgetsList(
      swipe_widget_, {frame_widgets_.begin(), frame_widgets_.end()});
  swipe_widget_->addSwipeView(swipe_view_);
  swipe_widget_->setGeometry(rect());

  connect(swipe_view_, &SwipeWidgetsList::SignalItemChanged, this,
          [&](int index) {
            const auto new_id = frame_widgets_[index]->innerWidget()->id();
            if (new_id != frame_control.CurrentProject()->frame_id_) {
              frame_control.CurrentProject()->frame_id_ = new_id;
              frame_control.CurrentProject()->state |=
                  (short)Core::Project::STATE::CHANGED;
            }
          });

  controls_.push_back(swipe_widget_);
}

QString MainWindow::SelectButtonText(int count) {
  if (count == 12 || count == 0) {
    return tr("Load photos");
  } else if (count != 1) {
    return tr("Load %1 photos").arg(count);
  } else {
    return tr("Load a photo");
  }
}
void MainWindow::CreateButtons(Core::FrameControl &control) {

  preview_button_ = new TextButton(this);
  preview_button_->setObjectName("White");
  preview_button_->setStyleSheet(c_white_button_style_str);

  preview_button_->setSize(QSize(120, 60));
  preview_button_->setText(tr("Preview"));
  //  preview_button_->setVisible(false);
  controls_.push_back(preview_button_);

  connect(preview_button_, &QPushButton::clicked, this, [&] {
    auto f = [&]() {
      QMetaObject::invokeMethod(
          this,
          [&]() {
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
          },
          Qt::QueuedConnection);
    };
    ShowLoadingDialogStub(f);

    preview_->show();
    setEnabledControls(false);
  });

  share_button_ = new ShareButton(this);
  // share_button_->setVisible(false);
  connect(share_button_, &QPushButton::clicked, this, [&] {
    auto f = [&]() {
      QMetaObject::invokeMethod(
          this,
          [&]() {
            const QPixmap pixmap = Render(control);
            Share(pixmap);
          },
          Qt::QueuedConnection);
    };
    ShowLoadingDialogStub(f);
  });

  controls_.push_back(share_button_);

  rearrange_button_ = new QPushButton(this);
  rearrange_button_->setObjectName("Rearrange");
  rearrange_button_->setStyleSheet(c_rearrange_button_str);
  rearrange_button_->setText(tr("Rearrange"));

  connect(rearrange_button_, &QPushButton::clicked, this,
          [&] { rearrange_->show(); });

  controls_.push_back(rearrange_button_);

  UpdateRearrangeButton();

  select_images_button_ = new TextButton(this);
  select_images_button_->setObjectName("Select");
  select_images_button_->setStyleSheet(c_select_button_style_str);

  connect(select_images_button_, &QPushButton::clicked, this, [&] {
    const auto files = FileDialog::getOpenFileNames();
    MainWindow::SelectImages(files);
  });

  controls_.push_back(select_images_button_);

  UpdateSelectionButton(project_control_);

  info_button_ = new TextButton(this);
  info_button_->setObjectName("Info");
  info_button_->setStyleSheet(c_info_button_style_str);
  info_button_->setText("i");
  ((TextButton *)info_button_)->setSize(QSize(40, 40));

  connect(info_button_, &QPushButton::clicked, this,
          [&] { info_widget_->show(); });

  controls_.push_back(select_images_button_);
}

void MainWindow::ShowLoadingDialogStub(std::function<void()> f) {

  QProgressDialog progress(tr("Loading..."), "", 0, 100, this);
  progress.setStyleSheet(c_progress_dialog_style_str);
  progress.setMinimumDuration(1);
  progress.setCancelButton(nullptr);
  progress.setWindowModality(Qt::ApplicationModal);
  progress.show();
  progress.setValue(10);

  f();

  progress.setValue(10 * QRandomGenerator::global()->bounded(4, 9));
}

void MainWindow::CreateTapText() {
  tap_text_ = new QLabel(this);
  tap_text_->setObjectName("TapText");
  tap_text_->setStyleSheet(c_tap_text_style_str);
  tap_text_->setText(tr("tap photo to edit"));
  tap_text_->setWordWrap(true);

  controls_.push_back(tap_text_);

  UpdateTapText();
}

bool MainWindow::SelectImages(QStringList files) {
  QProgressDialog progress(tr("Loading photos..."), "", 0, files.size(), this);
  progress.setStyleSheet(c_progress_dialog_style_str);
  progress.setMinimumDuration(500);
  progress.setCancelButton(nullptr);

  progress.setWindowModality(Qt::WindowModal);
  int count_loaded = 0;

  std::map<QString, std::vector<QString>> map;
  for (const auto &path : files) {
    QFileInfo file_info(path);
    if (!file_info.exists()) {
      spdlog::error("SelectImages invalid file");
      continue;
    }

    QString time;
    /*  if (file_info.birthTime().isValid()) {
        time = QString().setNum(file_info.birthTime().toSecsSinceEpoch());
      } else if (file_info.lastModified().isValid()) {
        time = QString().setNum(file_info.lastModified().toSecsSinceEpoch());
      } else*/
    { time = file_info.fileName(); }
    /*   qDebug() << "** birthTime " << file_info.birthTime();
       qDebug() << "** lastModified " << file_info.lastModified();
        qDebug() << "** fileName " << file_info.fileName();
      qDebug() << "** " << time;*/
    map[time].push_back(path);
  }

  auto project = project_control_.CurrentProject();

  size_t month = 0;
  for (const auto &[time, file_vector] : map) {
    for (auto file_it = file_vector.rbegin(); file_it != file_vector.rend();
         file_it++) {
      while (project->monthes_.size() > month) {
        auto &month_data = project->monthes_[month];
        month++;
        if (month_data.photo_data->isStub()) {
          const auto image_name =
              project_control_.imageManager()->addImage(*file_it);
          if (!image_name.isNull()) {
            month_data.photo_data->resetData(image_name, true);
            count_loaded++;
            progress.setValue(count_loaded);
          } else {
            month--;
            spdlog::error("SelectImages invalid image file");
          }
          break;
        }
      }
    }
  }
  UpdateFrames(nullptr);
  project_control_.SaveProject();
  progress.setValue(files.size());

  return !files.isEmpty();
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

void MainWindow::ShareLog() const {

  if (!share_utiles_) {
    share_utiles_ = std::make_shared<ShareUtils::ShareUtilsCpp>();
  }

  share_utiles_->sendFile(project_control_.logFilePath(), "View File",
                          c_share_text_mime_type_str, 1);
}

QPixmap MainWindow::Render(Core::FrameControl &control) {
  return FrameWidgetsFactory::renderWidget(control.CurrentProject()->frame_id_,
                                           this, control);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
  if (e) {

    if (!e->size().isValid() || e->size().isEmpty()) {
      return;
    }

    QMainWindow::resizeEvent(e);
  }

  const bool is_portrait = width() < height();

  if (background_svg_) {
    const int size = std::max(width(), height());
    background_svg_->setGeometry(QRect{QPoint{0, 0}, QSize{size, size}});
  }

  if (swipe_widget_) {
    swipe_widget_->setGeometry(rect());
  }

  if (photo_tune_widget_)
    photo_tune_widget_->setGeometry(rect());

  if (rearrange_)
    rearrange_->setGeometry(rect());

  if (info_widget_)
    info_widget_->setGeometry(rect());

  if (line_edit_)
    line_edit_->setGeometry(rect());

  if (preview_button_) {
    const int diff = share_button_->height() - preview_button_->height();
    preview_button_->setGeometry(
        {{height() / 50,
          height() - height() / 20 - share_button_->height() + diff / 2},
         preview_button_->size()});
  }

  int share_button_top = 0;
  if (share_button_) {
    share_button_top = height() - share_button_->height() - height() / 20;
    share_button_->setGeometry(
        {{width() - share_button_->width() - height() / 40, share_button_top},
         share_button_->size()});
  }

  int select_button_left = 0;
  if (select_images_button_) {
    select_button_left =
        width() - select_images_button_->width() - height() / 40;
    select_images_button_->setGeometry(
        {{select_button_left, height() / 20}, select_images_button_->size()});
  }

  if (preview_)
    preview_->setGeometry(rect());

  const int rearrange_button_width = 160;
  const int rearrange_button_height = 50;
  int rearrange_button_top = 0;
  if (rearrange_button_ && select_images_button_ && share_button_) {
    if (is_portrait) {
      rearrange_button_top = share_button_->geometry().top() - height() / 17;
    } else {
      rearrange_button_top = select_images_button_->geometry().top();
    }

    rearrange_button_->setGeometry(height() / 60, rearrange_button_top,
                                   rearrange_button_width,
                                   rearrange_button_height);
  }

  if (info_button_ && select_images_button_ && rearrange_button_) {
    int info_butoon_top =
        select_images_button_->geometry().top() +
        (select_images_button_->geometry().height() - info_button_->height()) /
            2;
    info_butoon_top =
        is_portrait
            ? info_butoon_top
            : (rearrange_button_->isVisible()
                   ? height() / 20 + rearrange_button_->geometry().bottom()
                   : select_images_button_->geometry().top());

    info_button_->setGeometry(
        {{height() / 55, info_butoon_top}, info_button_->size()});
  }

  if (tap_text_) {

    int tap_text_width = 0;
    int tap_text_top = 0;
    int tap_text_left = 0;
    int tap_text_height = 0;

    tap_text_->adjustSize();

    if (is_portrait) {
      tap_text_width = width() - 2 * height() / 40;
      tap_text_top = height() / 5.5;
      tap_text_left = height() / 40;
      tap_text_height = tap_text_->heightForWidth(tap_text_width);
    } else {
      tap_text_width = 180;
      tap_text_left = width() - tap_text_width;
      tap_text_height = tap_text_->heightForWidth(tap_text_width);
      tap_text_top = share_button_top - tap_text_height * 1.5;
    }

    tap_text_->setGeometry(tap_text_left, tap_text_top, tap_text_width,
                           tap_text_height);
  }

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

  if (line_edit_->isVisible()) {
    line_edit_->hide();
    setEnabledControls(true);
    event->ignore();
    return;
  }

  for (auto &widget : child_widgets_) {
    if (widget->isVisible()) {

      widget->hide();

      setEnabledControls(true);

      event->ignore();
      return;
    }
  }

  project_control_.SaveProject();
  QMainWindow::closeEvent(event);
}

void MainWindow::setEnabledControls(bool enabled) {
  for (auto &widgets : controls_) {
    widgets->setUpdatesEnabled(enabled);
    widgets->setEnabled(enabled);
  }

  if (enabled) {
    UpdateSelectionButton(project_control_);
  }
}

MainWindow::~MainWindow() {}
} // namespace FirstYear::UI
