#include "qswipeview.hpp"
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

FiltersScrollWidget::FiltersScrollWidget(
    QWidget *parent, FirstYear::Core::FrameControl &frame_control)
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

  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

  CreateInnerWidget(frame_control);

  spdlog::info("FiltersScrollWidget UI created");
}

void FiltersScrollWidget::CreateInnerWidget(
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
  //  layout_->setGeometry({QPoint(), parentWidget()->geometry().size()});
  // filter_buttons_widget->setLayout(filter_buttons_layout);

  // bind button with controls, add to layout
  /*  int num = 0;
    auto buttons_it = controls->find(Modules::FILTER_BUTTON_TAG);
    if (buttons_it != controls->end()) {
        while (buttons_it != controls->end() &&
               buttons_it->first == Modules::FILTER_BUTTON_TAG) {
            auto control = buttons_it->second;
            UNI_ENSURE_RETURN(control);

            // create buttons
            const auto button =
                new RadioButton(UIString(control->Parameters()),
                                Modules::FILTER_BUTTON_TAG,
    filter_buttons_widget); QRect button_rect = QRect(0, 0, button_width - 10,
    button_width); button->setGeometry(button_rect);
            button->setMinimumWidth(button_width);
            button->setMinimumHeight(button_width);
            button->setMaximumHeight(button_width);
            button->setMaximumWidth(button_width);
            button->setFlat(true);

            const int index = num %
    std::size(c_filter_buttons_text_color_str_arr); QString
    style_template(c_filter_button_style_template_str); QString style_with_args
    = style_template.arg(c_filter_buttons_text_color_str_arr[index]);
            // .arg(c_round_button_str);
            button->setStyleSheet(style_with_args);

            // bind button with control
            const auto ui_command = new UICommand(this, control);
            connect(button, &QPushButton::clicked, ui_command,
                    &UICommand::OnButtonClicked);
            connect(ui_command, &UICommand::SignalCommandAppyed, this,
                    &FiltersScrollWidget::SignalCommandAppyed);

            //  connect(this, &FiltersWidget::SignalImageOpened, button,
            //        &ImageButton::OnSignalEnable);

            // add button to layout
            layout_->addWidget(button);

            buttons_it++;
            num++;
        }
    }*/

  AddWidget(new FirstYear::UI::DefaultFrameWidget(
      *filter_buttons_widget, parentWidget(), frame_control));
  AddWidget(new FirstYear::UI::DefaultFrameWidget2(
      *filter_buttons_widget, parentWidget(), frame_control));
  AddWidget(new FirstYear::UI::DefaultFrameWidget(
      *filter_buttons_widget, parentWidget(), frame_control));
  setWidget(filter_buttons_widget);
}

void FiltersScrollWidget::AddWidget(FirstYear::UI::FrameWidgetBase *widget) {
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
QSwipeView::QSwipeView(QWidget *parent)
    : QWidget(parent), stacked_widget_(new QStackedWidget(this)) {
  setGeometry(parent->geometry());
  stacked_widget_->setGeometry(parent->geometry());
  static const char *c_image_widget_button_style_str =
      "QSwipeView{ background-color: rgba(255, 255, 255, 0);}";
  setStyleSheet(c_image_widget_button_style_str);
  // setAttribute(Qt::WA_TransparentForMouseEvents);
  raise();
}

QSwipeView::~QSwipeView() {}

/*
int QSwipeView::count() const {return stacked_widget_->count();}
int  QSwipeView::currentIndex() const {return stacked_widget_->currentIndex();}
int QSwipeView::addWidget(QWidget* widget) {stacked_widget_->addWidget(widget);}
void  QSwipeView::insertWidget(int index, QWidget* w)
{stacked_widget_->insertWidget(index,w);} int QSwipeView::indexOf( QWidget* w)
const {return stacked_widget_->indexOf(w);} QWidget* QSwipeView::widget(int
index) const {return stacked_widget_->widget(index);} void
QSwipeView::setCurrentIndex(int index)
{stacked_widget_->setCurrentIndex()index;}
*/
int QSwipeView::minSwipeDistance() const {
  // The minimum swipe distance required to move to the prev/next page
  return (m_swipeVertical ? stacked_widget_->frameRect().height()
                          : stacked_widget_->frameRect().width()) /
         3;
}

int QSwipeView::maxMoveJumpDistance() const {
  // The minimum swipe distance required to move to the prev/next page
  return 10;
}

int QSwipeView::animationSpeed() const { return m_animationSpeed; }

void QSwipeView::animationSpeed(int animationSpeed_) {
  m_animationSpeed = animationSpeed_;
}

int QSwipeView::swipeVelocity() const { return m_swipeVelocity; }

void QSwipeView::swipeVelocity(int swipeVelocity_) {
  m_swipeVelocity = swipeVelocity_;
}

bool QSwipeView::swipeVertical() const { return m_swipeVertical; }

void QSwipeView::swipeVertical(bool swipeVertical_) {
  m_swipeVertical = swipeVertical_;
}

bool QSwipeView::event(QEvent *event) {
  spdlog::info("****** test ");
  if (event->type() == QEvent::MouseButtonPress) {
    mousePressEvent1(dynamic_cast<QMouseEvent *>(event));
  } else if (event->type() == QEvent::MouseMove) {
    mouseMoveEvent1(dynamic_cast<QMouseEvent *>(event));
  } else if (event->type() == QEvent::MouseButtonPress) {
    mouseReleaseEvent1(dynamic_cast<QMouseEvent *>(event));
  }
  event->ignore();
  return false;
  QWidget::event(event);
}

void QSwipeView::mousePressEvent1(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    pressPos = event->pos();
    oldPos = event->pos();
    accumMovePos = 0;
    QWidget *w;
    if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() - 1)) !=
        nullptr) {
      w->setGeometry(0, 0, stacked_widget_->frameRect().width(),
                     stacked_widget_->frameRect().height());
      w->show();
      w->raise();
      w->move(QPoint{m_swipeVertical ? 0
                                     : stacked_widget_->currentWidget()->x() -
                                           stacked_widget_->frameRect().width(),
                     m_swipeVertical ? stacked_widget_->currentWidget()->y() -
                                           stacked_widget_->frameRect().height()
                                     : 0});
    }
    if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() + 1)) !=
        nullptr) {
      w->setGeometry(0, 0, stacked_widget_->frameRect().width(),
                     stacked_widget_->frameRect().height());
      w->show();
      w->raise();
      w->move(QPoint{m_swipeVertical ? 0
                                     : stacked_widget_->currentWidget()->x() +
                                           stacked_widget_->frameRect().width(),
                     m_swipeVertical ? stacked_widget_->currentWidget()->y() +
                                           stacked_widget_->frameRect().height()
                                     : 0});
    }
  }
  // event->ignore();
  // return;
  // event->setAccepted(false);
  // QWidget::mousePressEvent(event);
}

void QSwipeView::mouseMoveEvent1(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    if (stacked_widget_->count() >= 1) {
      const QPoint movePos{event->pos()};
      QPoint diff = movePos - oldPos;
      accumMovePos += m_swipeVertical ? diff.y() : diff.x();
      const int distance = accumMovePos;
      const int minDistance = minSwipeDistance();
      const int frameWidth = stacked_widget_->frameRect().width();
      const int frameHeight = stacked_widget_->frameRect().height();

      spdlog::info("****** distance {}", distance);
      int difference = m_swipeVertical ? diff.y() : diff.x();
      if (difference > maxMoveJumpDistance()) {
        QWidget::mouseMoveEvent(event);
        return;
      }

      QWidget *w;
      QPoint newPos{m_swipeVertical ? 0 : distance,
                    m_swipeVertical ? distance : 0};
      if ((distance > minDistance) && (stacked_widget_->currentIndex() == 0)) {
        newPos = QPoint{m_swipeVertical ? 0 : minDistance,
                        m_swipeVertical ? minDistance : 0};
      } else if ((distance < -minDistance) &&
                 (stacked_widget_->currentIndex() ==
                  (stacked_widget_->count() - 1))) {
        newPos = QPoint{m_swipeVertical ? 0 : -minDistance,
                        m_swipeVertical ? -minDistance : 0};
      }
      if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() - 1)) !=
          nullptr) {
        w->move(QPoint{m_swipeVertical ? 0 : newPos.x() - frameWidth,
                       m_swipeVertical ? newPos.y() - frameHeight : 0});
      }
      if ((w = stacked_widget_->currentWidget()) != nullptr) {
        w->move(newPos);
      }
      if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() + 1)) !=
          nullptr) {
        w->move(QPoint{m_swipeVertical ? 0 : newPos.x() + frameWidth,
                       m_swipeVertical ? newPos.y() + frameHeight : 0});
      }
    }
  }
  // oldPos = event->pos();
  // event->ignore();
  // return;
  // QWidget::mouseMoveEvent(event);
  oldPos = event->pos();
}

void QSwipeView::mouseReleaseEvent1(QMouseEvent *event) {
  // event->setAccepted(false);

  const QPoint movePos{event->pos()};
  QPoint diff = movePos - oldPos;
  accumMovePos += m_swipeVertical ? diff.y() : diff.x();
  const int distance = accumMovePos;

  const int minDistance = minSwipeDistance();
  const int velocity =
      m_swipeVertical ? event->point(event->pointCount() - 1).velocity().y()
                      : event->point(event->pointCount() - 1).velocity().x();
  const int minVelocity = swipeVelocity();
  const int frameWidth = stacked_widget_->frameRect().width();
  const int frameHeight = stacked_widget_->frameRect().height();

  QWidget *w;
  QParallelAnimationGroup *animgroup = new QParallelAnimationGroup;
  QPropertyAnimation *anim;
  QPoint prevWidgetNewPos;
  QPoint currWidgetNewPos;
  QPoint nextWidgetNewPos;

  // If the distance traveled is more than the minimum distance
  // threshold OR the velocity is greater than the velocity threshold,
  // then go to the next page.  Otherwise, bounce back to the origin.
  if (stacked_widget_->count() >= 1) {
    if (((distance > minDistance) || (velocity > minVelocity)) &&
        (stacked_widget_->currentIndex() >= 1)) {
      // go to previous page
      prevWidgetNewPos = {0, 0};
      currWidgetNewPos = {m_swipeVertical ? 0 : frameWidth,
                          m_swipeVertical ? frameHeight : 0};
      nextWidgetNewPos = {m_swipeVertical ? 0 : (2 * frameWidth),
                          m_swipeVertical ? (2 * frameHeight) : 0};
      goingToPage = stacked_widget_->currentIndex() - 1;
    } else if ((((distance < -minDistance)) || (velocity < -minVelocity)) &&
               (stacked_widget_->currentIndex() <
                (stacked_widget_->count() - 1))) {
      // go to next page
      prevWidgetNewPos = {m_swipeVertical ? 0 : (-2 * frameWidth),
                          m_swipeVertical ? (-2 * frameHeight) : 0};
      currWidgetNewPos = {m_swipeVertical ? 0 : -frameWidth,
                          m_swipeVertical ? -frameHeight : 0};
      nextWidgetNewPos = {0, 0};
      goingToPage = stacked_widget_->currentIndex() + 1;
    } else {
      // snap back to the beginning of the same page
      prevWidgetNewPos = {m_swipeVertical ? 0 : -frameWidth,
                          m_swipeVertical ? -frameHeight : 0};
      currWidgetNewPos = {0, 0};
      nextWidgetNewPos = {m_swipeVertical ? 0 : frameWidth,
                          m_swipeVertical ? frameHeight : 0};
      goingToPage = stacked_widget_->currentIndex();
    }

    if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() - 1)) !=
        nullptr) {
      anim = new QPropertyAnimation(w, "pos");
      anim->setDuration(m_animationSpeed);
      anim->setEasingCurve(QEasingCurve::OutQuart);
      anim->setStartValue(w->pos());
      anim->setEndValue(prevWidgetNewPos);
      animgroup->addAnimation(anim);
    }
    if ((w = stacked_widget_->currentWidget()) != nullptr) {
      anim = new QPropertyAnimation(w, "pos");
      anim->setDuration(m_animationSpeed);
      anim->setEasingCurve(QEasingCurve::OutQuart);
      anim->setStartValue(w->pos());
      anim->setEndValue(currWidgetNewPos);
      animgroup->addAnimation(anim);
    }
    if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() + 1)) !=
        nullptr) {
      anim = new QPropertyAnimation(w, "pos");
      anim->setDuration(m_animationSpeed);
      anim->setEasingCurve(QEasingCurve::OutQuart);
      anim->setStartValue(w->pos());
      anim->setEndValue(nextWidgetNewPos);
      animgroup->addAnimation(anim);
    }
    QObject::connect(animgroup, &QParallelAnimationGroup::finished, this,
                     &QSwipeView::onAnimationFinished);
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
  }
  // event->ignore();
  // return;
  // QWidget::mouseReleaseEvent(event);
}

void QSwipeView::onAnimationFinished() {
  QWidget *w;
  stacked_widget_->setCurrentIndex(goingToPage);
  if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() - 1)) !=
      nullptr) {
    w->hide();
  }
  if ((w = stacked_widget_->widget(stacked_widget_->currentIndex() + 1)) !=
      nullptr) {
    w->hide();
  }
  emit animationFinished();
}

bool QSwipeView::gotoPage(int index) {
  bool rval = false;

  if (stacked_widget_->count() >= 1 && 0 <= index &&
      index < stacked_widget_->count()) {
    const int frameWidth = stacked_widget_->frameRect().width();
    const int frameHeight = stacked_widget_->frameRect().height();

    QWidget *w;
    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup;
    QPropertyAnimation *anim;
    QPoint prevWidgetNewPos;
    QPoint currWidgetNewPos;
    QPoint nextWidgetNewPos;

    rval = true;

    if (index < stacked_widget_->currentIndex()) {
      // go to a previous page
      prevWidgetNewPos = {0, 0};
      currWidgetNewPos = {m_swipeVertical ? 0 : frameWidth,
                          m_swipeVertical ? frameHeight : 0};
      if ((w = stacked_widget_->widget(index)) != nullptr) {
        w->setGeometry(0, 0, frameWidth, frameHeight);
        w->show();
        w->raise();
        w->move(QPoint{m_swipeVertical ? 0 : -frameWidth,
                       m_swipeVertical ? -frameHeight : 0});
        anim = new QPropertyAnimation(w, "pos");
        anim->setDuration(m_animationSpeed);
        anim->setEasingCurve(QEasingCurve::OutQuart);
        anim->setStartValue(w->pos());
        anim->setEndValue(prevWidgetNewPos);
        animgroup->addAnimation(anim);
      }
      if ((w = stacked_widget_->currentWidget()) != nullptr) {
        anim = new QPropertyAnimation(w, "pos");
        anim->setDuration(m_animationSpeed);
        anim->setEasingCurve(QEasingCurve::OutQuart);
        anim->setStartValue(w->pos());
        anim->setEndValue(currWidgetNewPos);
        animgroup->addAnimation(anim);
      }
      goingToPage = index;
    } else if (index > stacked_widget_->currentIndex()) {
      // go to a later page
      currWidgetNewPos = {m_swipeVertical ? 0 : -frameWidth,
                          m_swipeVertical ? -frameHeight : 0};
      nextWidgetNewPos = {0, 0};
      if ((w = stacked_widget_->currentWidget()) != nullptr) {
        anim = new QPropertyAnimation(w, "pos");
        anim->setDuration(m_animationSpeed);
        anim->setEasingCurve(QEasingCurve::OutQuart);
        anim->setStartValue(w->pos());
        anim->setEndValue(currWidgetNewPos);
        animgroup->addAnimation(anim);
      }
      if ((w = stacked_widget_->widget(index)) != nullptr) {
        w->setGeometry(0, 0, frameWidth, frameHeight);
        w->show();
        w->raise();
        w->move(QPoint{m_swipeVertical ? 0 : frameWidth,
                       m_swipeVertical ? frameHeight : 0});
        anim = new QPropertyAnimation(w, "pos");
        anim->setDuration(m_animationSpeed);
        anim->setEasingCurve(QEasingCurve::OutQuart);
        anim->setStartValue(w->pos());
        anim->setEndValue(nextWidgetNewPos);
        animgroup->addAnimation(anim);
      }
      goingToPage = index;
    } else {
      goingToPage = stacked_widget_->currentIndex();
    }
    QObject::connect(animgroup, &QParallelAnimationGroup::finished, this,
                     &QSwipeView::onAnimationFinished);
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
  }
  return rval;
}

bool QSwipeView::gotoPrevPage() {
  return gotoPage(stacked_widget_->currentIndex() - 1);
}

bool QSwipeView::gotoNextPage() {
  return gotoPage(stacked_widget_->currentIndex() + 1);
}
