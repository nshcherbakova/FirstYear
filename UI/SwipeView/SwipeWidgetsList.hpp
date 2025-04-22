#pragma once

#include <Core/FrameControl/FrameControl.h>
#include <QtWidgets>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>

class QPropertyAnimation;
class QSvgRenderer;

namespace FirstYear::UI {
class SwipeWidgetsList final : public QScrollArea {
  Q_OBJECT
public:
  explicit SwipeWidgetsList(QWidget *parent,
                            const std::vector<QWidget *> &widgets);
  SwipeWidgetsList &operator=(const SwipeWidgetsList &) = delete;

public:
  void SetCurrentItem(int index);
  int CurrentItem() const;
  int Count() const;

signals:
  void SignalItemChanged(int index);
  void SignalItemAdded();

protected:
  // virtual bool event(QEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override final;
  /*#ifdef FIRSTYEAR_PROFILING
    virtual void paintEvent(QPaintEvent *) override final;
    virtual void mouseReleaseEvent(QMouseEvent *) override final;
  #endif*/

private:
  void InitialaizeScroller();
  void CreateInnerWidget(const std::vector<QWidget *> &widgets);
  void AddWidget(QWidget *widget);
  /*
  #ifdef FIRSTYEAR_PROFILING
    void onFpsTimeout();
    void onAnimationTimeout();
  #endif*/
private:
  QHBoxLayout *layout_ = nullptr;
  Qt::GestureType grabbed_gesture_;
  int current_item_index_ = 0;
  std::vector<QWidget *> frame_widgets_;

  // QSvgRenderer *left_swipe_arrow_ = nullptr;
  // QSvgRenderer *right_swipe_arrow_ = nullptr;
  /*
  #ifdef FIRSTYEAR_PROFILING
    QTimer timer;
    double measurement = 0;
    qint64 old_time = 0;
    QPropertyAnimation *animation = nullptr;
  #endif*/
};
} // namespace FirstYear::UI
