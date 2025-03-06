#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Core/FrameControl/FrameControl.h>
#include <QMainWindow>
#include <QPixmap>
#include <Types.h>

class QPushButton;
class QStackedLayout;
class QSvgWidget;
class QLabel;
class QSvgRenderer;

namespace ShareUtils {
class ShareUtilsCpp;
}

namespace FirstYear::UI {
class SwipeWidgetsList;
class PhotoTuneWidget;
class SwipeWidget;
class TemplateWidgetHolder;
class LineEditWidget;
class TextButton;
class RearrangeWidget;
class ClickableLabel;

namespace Preview {
class PreviewWidget;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(Core::FrameControl &frame_control, const QStringList &frames);
  ~MainWindow();

signals:
  void SignalBack();

private:
  void CreatePhotoTuneWidget(Core::FrameControl &frame_control);
  void CreateFrames(Core::FrameControl &frame_control,
                    const QStringList &frames);
  void CreateSwipeWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateLineEditWidget(FirstYear::Core::FrameControl &frame_control);
  void CreateButtons(FirstYear::Core::FrameControl &frame_control);
  // void CreateDragAndDropText(FirstYear::Core::FrameControl &frame_control);
  void CreatePreviewWindow();
  void CreateRearrangeWidget(Core::FrameControl &frame_control);

  void UpdateFrames(TemplateWidgetHolder *exept);
  int CurrentTemplateIndex(FirstYear::Core::FrameControl &frame_control) const;
  void UpdateSelectionButton(FirstYear::Core::FrameControl &frame_control);
  int LoadedPhotosCount(FirstYear::Core::FrameControl &frame_control) const;
  // void UpdateDrugAndDrop();
  void UpdateRearrangeButton();

  void TuneImage(int month, FirstYear::Core::FrameControl &frame_control);
  void SaveTunedImage(int month, FirstYear::Core::FrameControl &frame_control);
  void OpenImage(int month);
  void TuneNewImage(int current_month, int next_month,
                    FirstYear::Core::FrameControl &frame_control);

  QPixmap Render(Core::FrameControl &control);
  void Share(const QPixmap &pixmap) const;

  void OnImagePicked(QString file, int month);
  void SelectImages(QStringList files);

  void setEnabledControls(bool enabled);
  QString SelectButtonText(int count);

  void DeletePhoto(int month_index);

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;
  virtual void closeEvent(QCloseEvent *event) override final;
  virtual void keyReleaseEvent(QKeyEvent *event) override final;

private:
  FirstYear::Core::FrameControl &project_control_;
  mutable std::shared_ptr<ShareUtils::ShareUtilsCpp> share_utiles_;
  std::vector<TemplateWidgetHolder *> frame_widgets_;
  SwipeWidgetsList *swipe_view_ = nullptr;
  SwipeWidget *swipe_widget_ = nullptr;
  PhotoTuneWidget *photo_tune_widget_ = nullptr;
  LineEditWidget *line_edit_ = nullptr;
  Preview::PreviewWidget *preview_ = nullptr;

  TextButton *preview_button_ = nullptr;
  QPushButton *share_button_ = nullptr;
  QPushButton *select_images_button_ = nullptr;
  QStackedLayout *stackedLayout = nullptr;
  QSvgWidget *background_svg_ = nullptr;
  QPushButton *rearrange_button_ = nullptr;
  RearrangeWidget *rearrange_ = nullptr;
};
} // namespace FirstYear::UI
#endif // MAINWINDOW_H
