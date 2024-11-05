#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <QPixmap>
#include <Types.h>

class QPushButton;

namespace ShareUtils {
class ShareUtilsCpp;
}

namespace FirstYear::UI {
class SwipeWidgetsList;
class PhotoTuneWidget;
class SwipeWidget;
class TemplateWidgetHolder;
class LineEditWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control);
  ~MainWindow();

protected:
private:
  void CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(Core::FrameControl &frame_control);
  void CreateSwipeWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateLineEditWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateButtons(FirstYear::Core::FrameControl &frame_control);
  void UpdateFrames(TemplateWidgetHolder *exept);
  int CurrentTemplateIndex(FirstYear::Core::FrameControl &frame_control) const;

  void TuneImage(int month, FirstYear::Core::FrameControl &frame_control);
  void SaveTunedImage(int month, FirstYear::Core::FrameControl &frame_control);
  bool OpenImage(int month, FirstYear::Core::FrameControl &frame_control);
  void TuneNewImage(int current_month, int next_month,
                    FirstYear::Core::FrameControl &frame_control);
  QPixmap Render(Core::FrameControl &control);

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  // FirstYear::Core::FrameControl &project_control_;
  std::shared_ptr<ShareUtils::ShareUtilsCpp> share_utiles_;
  std::vector<TemplateWidgetHolder *> frame_widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  SwipeWidget *swipe_widget_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  LineEditWidget *line_edit_ = nullptr;

  QPushButton *render_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
