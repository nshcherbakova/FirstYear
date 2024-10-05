#include "SwipeWidgetsList.hpp"
#include "stdafx.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

static const char *c_scroll_style_str = "QScrollArea{"
                                        "background: transparent;"
                                        "border: none;"
                                        "}";
static const char *c_filter_widget_style_str = "QWidget{"
                                               "background: transparent;"
                                               "border: none;"
                                               "}";

SwipeWidgetsList::SwipeWidgetsList(
    QWidget *parent, std::vector<FirstYear::UI::FrameWidgetBase *> widgets)
    : QScrollArea(parent) {

  UNI_ASSERT(widgets.size() > 0);
  setStyleSheet(c_scroll_style_str);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContentsMargins(0, 0, 0, 0);
  InitialaizeScroller(widgets[0]->minimumWidth());
  CreateInnerWidget(std::move(widgets));
  spdlog::info("FiltersScrollWidget UI created");
}

void SwipeWidgetsList::InitialaizeScroller(int item_with) {
  QScrollerProperties properties =
      QScroller::scroller(this)->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                             QScrollerProperties::OvershootAlwaysOff);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                             QScrollerProperties::OvershootAlwaysOff);
  properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor,
                             0.6);
  properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
  properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5);
  properties.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime,
                             0.4);
  properties.setScrollMetric(
      QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
  properties.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
  properties.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity,
                             0);
  properties.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);

  QScroller::scroller(this)->setScrollerProperties(properties);
  QScroller::scroller(this)->setSnapPositionsX(0, item_with);
  QScroller::scroller(this)->scrollTo(QPoint());
  // this->horizontalScrollBar()->setValue(0);

  grabbed_gesture_ =
      QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

  connect(QScroller::scroller(this), &QScroller::stateChanged, this,
          [&](QScroller::State newState) {
            if (newState == QScroller::Inactive) {
              const auto currant_scroll_pos =
                  QScroller::scroller(this)->finalPosition();
              emit SignalItemChanged(currant_scroll_pos.x() / item_with);
            }
          });
}

void SwipeWidgetsList::CreateInnerWidget(
    std::vector<FirstYear::UI::FrameWidgetBase *> widgets) {

  // buttons widget
  QWidget *filter_buttons_widget = new QWidget();
  filter_buttons_widget->setContentsMargins(0, 0, 0, 0);
  filter_buttons_widget->setStyleSheet(c_filter_widget_style_str);
  //  filter_buttons_widget->setMinimumHeight(parentWidget()->geometry().height());

  // buttons layput
  layout_ = new QHBoxLayout(filter_buttons_widget);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);

  for (auto &widget : widgets) {
    widget->setParent(filter_buttons_widget);
    AddWidget(widget);
  }
  setWidget(filter_buttons_widget);

  // ensureWidgetVisible(widget);
}

void SwipeWidgetsList::SetCurrentItem(int index) {
  UNI_ASSERT(layout_->count() > index);
  this->horizontalScrollBar()->setValue(layout_->itemAt(0)->geometry().width() *
                                        index);
}

void SwipeWidgetsList::AddWidget(FirstYear::UI::FrameWidgetBase *widget) {

  widget->show();
  //   widget->setFlat(true);
  layout_->addWidget(widget);

  // update();
}

/*bool SwipeWidgetsList::event(QEvent *event) {

  if (false&&event->type() == QEvent::Gesture) {
    if (QGesture *gesture =
            static_cast<QGestureEvent *>(event)->gesture(grabbed_gesture_)) {
      switch (gesture->state()) {
      case Qt::GestureFinished:
      case Qt::GestureCanceled: {
        auto animation = [&] {
          auto current_value = this->horizontalScrollBar()->value();
          int end_val = 0;
          if (current_value % width() > width() / 2) {
            end_val = (current_value / width() + 1) * width();

          } else {
            end_val = (current_value / width()) * width();
          }

          QScroller::scroller(this)->scrollTo(QPoint(end_val, 0));
        };
        QTimer::singleShot(400, this, animation);
      } break;
      default:
        return QScrollArea::event(event);
      }
    }
  }
  return QScrollArea::event(event);
}*/
