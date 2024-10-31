#include "SwipeWidgetsList.hpp"
#include "stdafx.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace FirstYear::UI {
static const char *c_scroll_style_str = "QScrollArea{"
                                        "background: transparent;"
                                        "border: none;"
                                        "}";
static const char *c_filter_widget_style_str = "QWidget{"
                                               "background: transparent;"
                                               "border: none;"
                                               "}";

SwipeWidgetsList::SwipeWidgetsList(
    QWidget *parent, const std::vector<TemplateWidgetBase *> &widgets)
    : QScrollArea(parent) {

  UNI_ASSERT(widgets.size() > 0);
  setStyleSheet(c_scroll_style_str);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContentsMargins(0, 0, 0, 0);
  setWidgetResizable(true);
  CreateInnerWidget(widgets);
  InitialaizeScroller();
  spdlog::info("FiltersScrollWidget UI created");
}

void SwipeWidgetsList::InitialaizeScroller() {
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
  QScroller::scroller(this)->setSnapPositionsX(0, width());
  QScroller::scroller(this)->scrollTo(QPoint());

  grabbed_gesture_ =
      QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

  connect(QScroller::scroller(this), &QScroller::stateChanged, this,
          [&](QScroller::State newState) {
            if (newState == QScroller::Inactive) {
              const auto currant_scroll_pos =
                  QScroller::scroller(this)->finalPosition();
              current_item_index_ = currant_scroll_pos.x() / width();
              emit SignalItemChanged(current_item_index_);
            }
          });
}

void SwipeWidgetsList::CreateInnerWidget(
    const std::vector<TemplateWidgetBase *> &widgets) {

  // buttons widget
  QWidget *filter_buttons_widget = new QWidget();
  filter_buttons_widget->setContentsMargins(0, 0, 0, 0);
  filter_buttons_widget->setStyleSheet(c_filter_widget_style_str);

  // buttons layout
  layout_ = new QHBoxLayout(filter_buttons_widget);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);

  for (auto &widget : widgets) {
    widget->setParent(filter_buttons_widget);
    AddWidget(widget);
  }
  setWidget(filter_buttons_widget);
}

void SwipeWidgetsList::resizeEvent(QResizeEvent *e) {

  QScrollArea::resizeEvent(e);
  QScroller::scroller(this)->setSnapPositionsX(0, width());
  layout_->update();

  SetCurrentItem(current_item_index_);
}

void SwipeWidgetsList::SetCurrentItem(int index) {
  current_item_index_ = index;
  UNI_ASSERT(layout_->count() > index);
  QScroller::scroller(this)->scrollTo(QPoint(width() * index, 0), 0);
}

int SwipeWidgetsList::CurrentItem() const { return current_item_index_; }

void SwipeWidgetsList::AddWidget(TemplateWidgetBase *widget) {

  widget->show();
  layout_->addWidget(widget);
}
} // namespace FirstYear::UI
