#pragma once

#include <QtWidgets>

#include <Core/FrameControl/FrameControl.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

class SwipeWidgetsList final : public QScrollArea {
  Q_OBJECT
public:
  explicit SwipeWidgetsList(
      QWidget *parent, std::vector<FirstYear::UI::FrameWidgetBase *> widgets);
  SwipeWidgetsList &operator=(const SwipeWidgetsList &) = delete;

public:
  void SetCurrentItem(int index);
signals:
  void SignalItemChanged(int index);

protected:
  // virtual bool event(QEvent *event) override;

public:
  void AddWidget(FirstYear::UI::FrameWidgetBase *widget);

private:
  void InitialaizeScroller(int item_with);
  void CreateInnerWidget(std::vector<FirstYear::UI::FrameWidgetBase *> widgets);

private:
  QHBoxLayout *layout_ = nullptr;
  Qt::GestureType grabbed_gesture_;
};
