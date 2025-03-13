#include "RearrangeWidget.h"
#include "PhotoWidget.h"
#include <UI/FrameData/FrameDataReader.h>
#include <stdafx.h>

namespace FirstYear::UI {

static const char *c_title_defoult_text_str = "Rearrange";

RearrangeWidget::RearrangeWidget(QWidget *parent, Core::FrameControl &control)
    : QWidget(parent), control_(control) {
  setObjectName("Rearrange");
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet("background-color: rgb(230, 230, 230)");

  background_svg_ = new QSvgWidget(":/images/icons/stars", this);

  createTitleTextWidget();
  createMonthPhotoWidgets();
  createPhotoTextWidgets();

  close_ = new TextButton(this);
  close_->setText("Ok");
  close_->setSize(QSize(110, 60));
  close_->setObjectName("LightButton");
  close_->setStyleSheet(c_light_button_style_str);
  connect(close_, &QPushButton::clicked, this, [&]() { hide(); });

  delete_ = new TextButton(this);
  delete_->setText("Delete");
  delete_->setObjectName("Delete");
  delete_->setStyleSheet(c_delete_button_style_str);
  connect(delete_, &QPushButton::clicked, this, [&]() {
    std::vector<int> remive_ids;
    for (int i = 0; i < (int)photo_widgets_.size(); i++) {
      if (photo_widgets_[i]->isChecked()) {
        remive_ids.push_back(i);
      }
    }
    emit SignalDeleteButtonClicked(remive_ids);
  });
  delete_->setDisabled(true);
}

void RearrangeWidget::createTitleTextWidget() {
  title_text_widget_ = new QLabel(this);

  title_text_widget_->setStyleSheet(
      "background-color: transparent; color:#555C24; ");
  title_text_widget_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void RearrangeWidget::createPhotoTextWidgets() {

  photo_text_widgets_.resize(12);
  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {
    auto &widget = photo_text_widgets_[i];

    widget = new QLabel(this);
    widget->setAlignment(Qt::AlignCenter);
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    widget->setStyleSheet("background-color: transparent; color:#555C24;");
  }
}

void RearrangeWidget::createMonthPhotoWidgets() {

  photo_widgets_.resize(12);

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i] =
        new PhotoWidget(*this, {true, true, false, false, false});
    connect(photo_widgets_[i], &PhotoWidget::SignalImageDroped, this,
            [&, i](int dropped_index) {
              emit SignalImageDroped(dropped_index, i);
            });

    connect(photo_widgets_[i], &PhotoWidget::toggled, this,
            [&]() { delete_->setEnabled(arePhotosSelected()); });
  }
}

void RearrangeWidget::Update() {
  load(control_);
  clearChecked();
  delete_->setDisabled(true);
}

void RearrangeWidget::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  Update();

  int top_margin = height() / 5;
  int side_margins = width() / 20;
  QRect rect = {side_margins, 0, width() - 2 * side_margins, top_margin};
  title_text_widget_->setGeometry(rect);
  title_text_widget_->setText(c_title_defoult_text_str);

  auto font = title_text_widget_->font();
  font.setPointSize(width() < height() ? top_margin / 4 : top_margin / 2);
  title_text_widget_->setFont(font);

  if (c_background_str) {
    const int size = std::max(width(), height());
    background_svg_->setGeometry(QRect{QPoint{0, 0}, QSize{size, size}});
  }

  close_->setGeometry(
      {{width() - close_->width() - height() / 40, height() / 20},
       close_->size()});
}

void RearrangeWidget::load(Core::FrameControl &control) {

  const bool vertical_orientation = width() < height();

  std::vector<QRect> photo_slots(12);
  int top_margin = height() / 5;
  int side_margins = width() / 20;
  int wmargins = width() / 20;
  int hmargins = height() / 20;

  int count_in_line = vertical_orientation ? 4 : 6;
  int photo_width = 0;
  int photo_height = 0;

  if (vertical_orientation) {
    top_margin = height() / 4;
    photo_width =
        (width() - 2 * side_margins - (count_in_line - 1) * wmargins) /
        count_in_line;
    photo_height = photo_width * 5.0 / 4.0;
  } else {
    int lines_count = (int)(photo_slots.size() / count_in_line);
    photo_height = (height() - 2 * top_margin - (lines_count - 1) * hmargins) /
                   lines_count;
    photo_width = photo_height * 4.0 / 5.0;
    wmargins = (width() - 2 * side_margins - count_in_line * photo_width) /
               (count_in_line - 1);
  }

  int k = 0;
  for (int j = 0; j < (int)(photo_slots.size() / count_in_line); j++) {
    for (int i = 0; i < count_in_line; i++) {

      photo_slots[k] = {side_margins + i * (wmargins + photo_width),
                        top_margin + j * (hmargins + photo_height), photo_width,
                        photo_height};
      k++;
    }
  }

  InitPhotos(control, photo_slots);

  auto project = control.CurrentProject();

  for (int i = 0; i < (int)photo_text_widgets_.size(); i++) {
    auto &month = project->monthes_[i];
    auto &photo_text_widget = photo_text_widgets_[i];

    photo_text_widget->setText(month.text);

    QRect rect = {photo_slots[i].left(), photo_slots[i].bottom(),
                  photo_slots[i].width(),
                  vertical_orientation ? int(hmargins * 2.0 / 3.0) : hmargins};

    photo_text_widget->setGeometry(rect);
    auto font = photo_text_widget->font();
    font.setPointSize(vertical_orientation ? hmargins * 0.4 : hmargins * 0.8);
    photo_text_widget->setFont(font);
  }

  delete_->setSize(!vertical_orientation ? QSize(80, 40) : QSize(110, 60));
  const int delete_top =
      (vertical_orientation ? height() - delete_->height() - hmargins * 1.4
                            : height() - delete_->height() - hmargins / 2);

  delete_->setGeometry({{side_margins, delete_top}, delete_->size()});
}

void RearrangeWidget::InitPhotos(Core::FrameControl &control,
                                 const std::vector<QRect> &photo_slots) {
  for (int i = 0; i < (int)photo_widgets_.size(); i++) {

    photo_widgets_[i]->setGeometry(photo_slots[i]);

    auto &month = control.CurrentProject()->monthes_[i];
    auto &photo_widget = photo_widgets_[i];

    photo_widget->setPhoto(month.photo_data, i);
    photo_widget->show();
  }
}

void RearrangeWidget::setVisible(bool visible) {
  if (visible) {
    Update();
  }
  QWidget::setVisible(visible);
}

void RearrangeWidget::clearChecked() {

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    photo_widgets_[i]->setChecked(false);
  }
}

bool RearrangeWidget::arePhotosSelected() const {

  for (int i = 0; i < (int)photo_widgets_.size(); i++) {
    if (photo_widgets_[i]->isChecked()) {
      return true;
    }
  }
  return false;
}
} // namespace FirstYear::UI
