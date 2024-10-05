#pragma once

#include <Core/FrameControl/FrameControl.h>
#include <QtWidgets>
#include <UI/FrameWidgets/DefaultFrameWidget.h>

namespace FirstYear::UI {
class SwipeWidgetsList final : public QScrollArea {
  Q_OBJECT
public:
  explicit SwipeWidgetsList(QWidget *parent,
                            const std::vector<FrameWidgetBase *> &widgets);
  SwipeWidgetsList &operator=(const SwipeWidgetsList &) = delete;

public:
  void SetCurrentItem(int index);
signals:
  void SignalItemChanged(int index);

protected:
  // virtual bool event(QEvent *event) override;

public:
  void AddWidget(FrameWidgetBase *widget);

private:
  void InitialaizeScroller(int item_with);
  void CreateInnerWidget(const std::vector<FrameWidgetBase *> &widgets);

private:
  QHBoxLayout *layout_ = nullptr;
  Qt::GestureType grabbed_gesture_;
};
} // namespace FirstYear::UI
