#pragma once

#include <Core/FrameControl/FrameControl.h>
#include <QtWidgets>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>

namespace FirstYear::UI {
class SwipeWidgetsList final : public QScrollArea {
  Q_OBJECT
public:
  explicit SwipeWidgetsList(QWidget *parent,
                            const std::vector<TemplateWidgetBase *> &widgets);
  SwipeWidgetsList &operator=(const SwipeWidgetsList &) = delete;

public:
  void SetCurrentItem(int index);
signals:
  void SignalItemChanged(int index);

protected:
  // virtual bool event(QEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override final;

public:
  void AddWidget(TemplateWidgetBase *widget);

private:
  void InitialaizeScroller();
  void CreateInnerWidget(const std::vector<TemplateWidgetBase *> &widgets);

private:
  QHBoxLayout *layout_ = nullptr;
  Qt::GestureType grabbed_gesture_;
  int current_item_index_ = 0;
};
} // namespace FirstYear::UI
