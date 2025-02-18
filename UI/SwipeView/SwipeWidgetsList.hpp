#pragma once

#include <Core/FrameControl/FrameControl.h>
#include <QtWidgets>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>

class QPropertyAnimation;

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
signals:
  void SignalItemChanged(int index);

protected:
  // virtual bool event(QEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override final;
  virtual void paintEvent(QPaintEvent *) override final;
  virtual void mouseReleaseEvent(QMouseEvent *) override final;

private:
  void InitialaizeScroller();
  void CreateInnerWidget(const std::vector<QWidget *> &widgets);
  void AddWidget(QWidget *widget);

  void onFpsTimeout();
  void onAnimationTimeout();

private:
  QHBoxLayout *layout_ = nullptr;
  Qt::GestureType grabbed_gesture_;
  int current_item_index_ = 0;
  std::vector<QWidget *> frame_widgets_;

  QTimer timer;
  double measurement = 0;
  qint64 old_time = 0;
  QPropertyAnimation *animation = nullptr;
};
} // namespace FirstYear::UI
