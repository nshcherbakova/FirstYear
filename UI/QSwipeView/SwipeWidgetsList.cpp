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

SwipeWidgetsList::SwipeWidgetsList(QWidget *parent,
                                   FirstYear::Core::FrameControl &frame_control)
    : QScrollArea(parent) {

  setStyleSheet(c_scroll_style_str);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContentsMargins(0, 0, 0, 0);
  // setGeometry({QPoint(), parentWidget()->geometry().size()});

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
  QScroller::scroller(this)->setSnapPositionsX(0, parentWidget()->width());

  grabbed_gesture_ =
      QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

  CreateInnerWidget(frame_control);

  spdlog::info("FiltersScrollWidget UI created");
}

void SwipeWidgetsList::CreateInnerWidget(
    FirstYear::Core::FrameControl &frame_control) {

  // buttons widget
  QWidget *filter_buttons_widget = new QWidget();
  filter_buttons_widget->setContentsMargins(0, 0, 0, 0);
  filter_buttons_widget->setStyleSheet(c_filter_widget_style_str);
  //  filter_buttons_widget->setMinimumHeight(parentWidget()->geometry().height());

  // buttons layput
  layout_ = new QHBoxLayout(filter_buttons_widget);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);

  FirstYear::UI::DefaultFrameWidget2 *widget =
      new FirstYear::UI::DefaultFrameWidget2(*filter_buttons_widget,
                                             parentWidget(), frame_control);

  AddWidget(new FirstYear::UI::DefaultFrameWidget(
      *filter_buttons_widget, parentWidget(), frame_control));
  AddWidget(widget);
  AddWidget(new FirstYear::UI::DefaultFrameWidget(
      *filter_buttons_widget, parentWidget(), frame_control));

  AddWidget(new FirstYear::UI::DefaultFrameWidget(
      *filter_buttons_widget, parentWidget(), frame_control));

  setWidget(filter_buttons_widget);

  ensureWidgetVisible(widget);

  this->horizontalScrollBar()->setValue(0);

  /*
  connect(QScroller::scroller(this), &QScroller::stateChanged, this,
  [&](QScroller::State newState){ if(newState == QScroller::Inactive)
      {
          auto current_value = this->horizontalScrollBar()->value();
          int end_val = 0;
          if (current_value % width() > width() / 2) {
              end_val = (current_value / width() + 1) * width();

          } else {
              end_val = (current_value / width()) * width();
          }

          QScroller::scroller(this)->scrollTo(QPoint(end_val, 0));
      }
  });*/
}

void SwipeWidgetsList::AddWidget(FirstYear::UI::FrameWidgetBase *widget) {

  QRect rect = QRect(0, 0, parentWidget()->geometry().width(),
                     parentWidget()->geometry().height());
  widget->setGeometry(rect);
  widget->setMinimumWidth(rect.width());
  widget->setMinimumHeight(rect.width());
  widget->setMaximumHeight(rect.width());
  widget->setMaximumWidth(rect.width());
  widget->show();
  //   widget->setFlat(true);
  layout_->addWidget(widget);
  widgets_.push_back(widget);
  connect(widget, &FirstYear::UI::FrameWidgetBase::SignalUpdate, this, [&] {
    for (auto w : widgets_) {

      w->Update();
    }
  });
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
