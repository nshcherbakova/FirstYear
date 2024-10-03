#pragma once

#include <QtWidgets>

#include <Core/FrameControl/FrameControl.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>
class FiltersScrollWidget final : public QScrollArea {
  Q_OBJECT
public:
  explicit FiltersScrollWidget(QWidget *parent,
                               FirstYear::Core::FrameControl &frame_control);
  FiltersScrollWidget &operator=(const FiltersScrollWidget &) = delete;

public:
  void AddWidget(FirstYear::UI::FrameWidgetBase *widget);

private:
  void CreateInnerWidget(FirstYear::Core::FrameControl &frame_control);

private:
  QHBoxLayout *layout_ = nullptr;
  std::vector<FirstYear::UI::FrameWidgetBase *> widgets_;
};

class QSwipeView : public QWidget {
  Q_OBJECT
  Q_PROPERTY(int animationSpeed MEMBER m_animationSpeed READ animationSpeed
                 WRITE animationSpeed);
  Q_PROPERTY(int swipeVelocity MEMBER m_swipeVelocity READ swipeVelocity WRITE
                 swipeVelocity);
  Q_PROPERTY(bool swipeVertical MEMBER m_swipeVertical READ swipeVertical WRITE
                 swipeVertical);

public:
  explicit QSwipeView(QWidget *parent = nullptr);
  ~QSwipeView();

public:
  QStackedWidget *stacked_widget_;
  /* int count() const;
   int  currentIndex() const;
   int addWidget(QWidget*);
   void  insertWidget(int index, QWidget* w);
   int indexOf( QWidget* w) const;
   QWidget* widget(int index) const;
   void  setCurrentIndex(int index);
   void  setCurrentIndex(int index);
 */
public:
  void animationSpeed(int animationSpeed_);
  int animationSpeed() const;
  void swipeVelocity(int swipeVelocity_);
  int swipeVelocity() const;
  void swipeVertical(bool swipeVertical_);
  bool swipeVertical() const;
  bool gotoPage(int index);
  bool gotoNextPage();
  bool gotoPrevPage();

signals:
  void animationFinished(void);

protected:
  bool event(QEvent *event);
  void mousePressEvent1(QMouseEvent *event);
  void mouseMoveEvent1(QMouseEvent *event);
  void mouseReleaseEvent1(QMouseEvent *event);

private:
  int minSwipeDistance() const;
  int maxMoveJumpDistance() const;
  void onAnimationFinished();

private:
private:
  bool m_swipeVertical{false};
  QPoint pressPos{0, 0};
  QPoint oldPos{0, 0};
  int accumMovePos = 0;
  int m_animationSpeed{300};
  int m_swipeVelocity{100};
  int goingToPage{-1};
};
