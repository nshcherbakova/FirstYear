#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <QPixmap>
#include <Types.h>

class QPushButton;
class QStackedLayout;

namespace ShareUtils {
class ShareUtilsCpp;
}

namespace FirstYear::UI {
class SwipeWidgetsList;
class PhotoTuneWidget;
class SwipeWidget;
class TemplateWidgetHolder;
class LineEditWidget;

namespace Preview {
class PreviewWidget;
}
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control);
  ~MainWindow();

signals:
  void SignalBack();

protected:
private:
  void CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(Core::FrameControl &frame_control);
  void CreateSwipeWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateLineEditWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateButtons(FirstYear::Core::FrameControl &frame_control);
  void CreatePreviewWindow();

  void UpdateFrames(TemplateWidgetHolder *exept);
  int CurrentTemplateIndex(FirstYear::Core::FrameControl &frame_control) const;
  void UpdateSelectionButton(FirstYear::Core::FrameControl &frame_control);

  void TuneImage(int month, FirstYear::Core::FrameControl &frame_control);
  void SaveTunedImage(int month, FirstYear::Core::FrameControl &frame_control);
  bool OpenImage(int month, FirstYear::Core::FrameControl &frame_control);
  void TuneNewImage(int current_month, int next_month,
                    FirstYear::Core::FrameControl &frame_control);

  QPixmap Render(Core::FrameControl &control);
  void Share(const QPixmap &pixmap) const;

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;
  virtual void closeEvent(QCloseEvent *event) override final;

private:
  FirstYear::Core::FrameControl &project_control_;
  mutable std::shared_ptr<ShareUtils::ShareUtilsCpp> share_utiles_;
  std::vector<TemplateWidgetHolder *> frame_widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  SwipeWidget *swipe_widget_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  LineEditWidget *line_edit_ = nullptr;
  Preview::PreviewWidget *preview_ = nullptr;

  QPushButton *preview_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
  QPushButton *select_images_button_ = nullptr;
  QStackedLayout *stackedLayout = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
