#pragma once

#include <QtWidgets>

#include <Core/FrameControl/FrameControl.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

class SwipeWidgetsList final : public QScrollArea {
  Q_OBJECT
public:
  explicit SwipeWidgetsList(QWidget *parent,
                            FirstYear::Core::FrameControl &frame_control);
  SwipeWidgetsList &operator=(const SwipeWidgetsList &) = delete;

protected:
  // virtual bool event(QEvent *event) override;

public:
  void AddWidget(FirstYear::UI::FrameWidgetBase *widget);

private:
  void CreateInnerWidget(FirstYear::Core::FrameControl &frame_control);

private:
  QHBoxLayout *layout_ = nullptr;
  std::vector<FirstYear::UI::FrameWidgetBase *> widgets_;
  Qt::GestureType grabbed_gesture_;
};
