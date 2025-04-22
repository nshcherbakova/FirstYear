#include "SwipeWidgetsList.hpp"
#include "stdafx.h"

#ifdef FIRSTYEAR_PROFILING
#include <QPropertyAnimation>
#endif

namespace FirstYear::UI {
static const char *c_scroll_style_str = "QScrollArea{"
                                        "background: transparent;"
                                        "border: none;"
                                        "}";

static const char *c_scroll_bar_style_str =
    "::corner {"
    "    background-color: rgb(58,57,58); /* dark */"
    "border: none;"
    "}"
    "QScrollBar:horizontal {"
    "    background-color: transparent;"
    "height: 14px;"
    "}"

    "QScrollBar::handle:horizontal {"
    "    background-color: #BC9F76;"
    "  min-width: 20px;"
    "margin: 2px;"
    "    border-radius: 5px;"
    "    border-width: 1px;"
    "border: 1px solid #BC9F76;; /* veryDark */"
    "}"

    "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
    "border: none;"
    "background: none;"
    "}"

    "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal, "
    "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
    "border: none;"
    "background: none;"
    "color: none;"
    "}";

static const char *c_frames_widget_style_str = "QWidget{"
                                               "background: transparent;"
                                               "border: none;"
                                               "}";

SwipeWidgetsList::SwipeWidgetsList(QWidget *parent,
                                   const std::vector<QWidget *> &widgets)
    : QScrollArea(parent) {

  UNI_ASSERT(widgets.size() > 0);
  setStyleSheet(c_scroll_style_str);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  horizontalScrollBar()->setStyleSheet(c_scroll_bar_style_str);
  setContentsMargins(0, 0, 0, 0);
  setWidgetResizable(true);
  CreateInnerWidget(widgets);
  InitialaizeScroller();
  spdlog::info("FiltersScrollWidget UI created");
  /*
  #ifdef FIRSTYEAR_PROFILING
    timer.setInterval(500);
    connect(&timer, &QTimer::timeout, this, [&]() { onFpsTimeout(); });
    timer.start();

    QTimer::singleShot(2 * 1000, this, [&]() { onAnimationTimeout(); });
  #endif*/
}

void SwipeWidgetsList::InitialaizeScroller() {
  QScrollerProperties properties =
      QScroller::scroller(this)->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                             QScrollerProperties::OvershootAlwaysOff);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                             QScrollerProperties::OvershootAlwaysOn);
  properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor,
                             0.2);
  properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
  properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.1);
  properties.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime,
                             0.4);
  properties.setScrollMetric(
      QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
  properties.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
  properties.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity,
                             0);
  properties.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);
  // properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.1);
  //  properties.setScrollMetric(QScrollerProperties::FrameRate,
  //  QScrollerProperties::Fps30);
  // properties.setScrollMetric(QScrollerProperties::ScrollingCurve,
  // QEasingCurve::OutCubic);

  QScroller::scroller(this)->setScrollerProperties(properties);
  QScroller::scroller(this)->setSnapPositionsX(0, width() + layout_->spacing());
  QScroller::scroller(this)->scrollTo(QPoint());

  grabbed_gesture_ =
      QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

  connect(QScroller::scroller(this), &QScroller::stateChanged, this,
          [&](QScroller::State newState) {
            if (newState == QScroller::Inactive) {
              const auto currant_scroll_pos =
                  QScroller::scroller(this)->finalPosition();
              current_item_index_ =
                  currant_scroll_pos.x() / (width() + layout_->spacing());
              emit SignalItemChanged(current_item_index_);
            }
          });
}

void SwipeWidgetsList::CreateInnerWidget(
    const std::vector<QWidget *> &widgets) {

  // buttons widget
  QWidget *frames_widget = new QWidget();
  frames_widget->setContentsMargins(0, 0, 0, 0);
  frames_widget->setStyleSheet(c_frames_widget_style_str);

  // buttons layout
  layout_ = new QHBoxLayout(frames_widget);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(20);

  for (auto &widget : widgets) {
    widget->setParent(frames_widget);
    AddWidget(widget);
  }
  setWidget(frames_widget);
}

void SwipeWidgetsList::resizeEvent(QResizeEvent *event) {
  for (auto &frame_widget : frame_widgets_) {
    frame_widget->setGeometry(QRect{{}, event->size()});
    frame_widget->setMinimumWidth(event->size().width());
    frame_widget->setMinimumHeight(event->size().height());
    frame_widget->setMaximumWidth(event->size().width());
    frame_widget->setMaximumHeight(event->size().height());
  }

  QScrollArea::resizeEvent(event);

  QScroller::scroller(this)->setSnapPositionsX(0, width() + layout_->spacing());
  layout_->update();
  SetCurrentItem(current_item_index_);
}

void SwipeWidgetsList::SetCurrentItem(int index) {
  current_item_index_ = index;
  UNI_ASSERT(layout_->count() > index);
  QScroller::scroller(this)->scrollTo(
      QPoint((width() + layout_->spacing()) * index, 0), 0);
  emit SignalItemChanged(current_item_index_);
}

int SwipeWidgetsList::CurrentItem() const { return current_item_index_; }

void SwipeWidgetsList::AddWidget(QWidget *widget) {

  widget->show();
  layout_->addWidget(widget);
  frame_widgets_.push_back(widget);

  emit SignalItemAdded();
}

int SwipeWidgetsList::Count() const { return (int)frame_widgets_.size(); }

/*#ifdef FIRSTYEAR_PROFILING
void SwipeWidgetsList::paintEvent(QPaintEvent *painter) {

  QScrollArea::paintEvent(painter);
  const auto current_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
  const auto current = current_time - old_time;
  float smoothing = 0.9; // larger=more smoothing
  measurement = (measurement * smoothing) + (current * (1.0 - smoothing));
  old_time = current_time;
}

void SwipeWidgetsList::onAnimationTimeout() {
  static int count = 1;
  static bool front = true;
  static int duration = 10000;

  animation = new QPropertyAnimation(horizontalScrollBar(), "value", this);
  animation->setDuration(duration);

  connect(animation, &QPropertyAnimation::finished, this, [&]() {
    if (count <= 5) {
      animation->state();
      onAnimationTimeout();
    } else {
      timer.stop();
    }
  });

  if (front) {
    animation->setStartValue(0);
    animation->setEndValue(horizontalScrollBar()->maximum());
  } else {

    animation->setEndValue(0);
    animation->setStartValue(horizontalScrollBar()->maximum());
  }
  animation->start();

  front = !front;
  count++;
  duration = duration / 2;
}

void SwipeWidgetsList::onFpsTimeout() {
  qDebug() << "Fps: " << 1000.0 / measurement;
}

void SwipeWidgetsList::mouseReleaseEvent(QMouseEvent *event) {
  QScrollArea::mouseReleaseEvent(event);
  animation->stop();
  timer.stop();
}
#endif*/
} // namespace FirstYear::UI
