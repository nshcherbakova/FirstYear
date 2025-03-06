#pragma once
#ifndef FIRSTYEAR_REARRANGE_WIDGET_H
#define FIRSTYEAR_REARRANGE_WIDGET_H
#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>

#include <Types.h>
#include <UI/FrameData/PhotoFrameParameters.h>
#include <UI/Utility.h>

#include <QWidget>

class QPushButton;
class QLabel;
class QSvgWidget;

namespace FirstYear::UI {
class TextButton;

class RearrangeWidget : public QWidget {
  Q_OBJECT
public:
  explicit RearrangeWidget(QWidget *parent, Core::FrameControl &control);

  RearrangeWidget &operator=(const RearrangeWidget &) = delete;
  virtual ~RearrangeWidget(){};

signals:
  void SignalRemoveButtonClicked(int month);
  void SignalImageDroped(int from_id, int to_id);
  void SignalDeleteButtonClicked(std::vector<int> monthes);

public slots:
  void Update();

public:
  QPixmap renderFrame();

public: // QWidget
  virtual void setVisible(bool visible) override final;

protected:
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  void createMonthPhotoWidgets();
  void createTitleTextWidget();
  void createPhotoTextWidgets();
  void InitPhotos(Core::FrameControl &control,
                  const std::vector<QRect> &photo_slots);
  void load(Core::FrameControl &control);
  void clearChecked();
  bool arePhotosSelected() const;

private:
  Core::FrameControl &control_;
  std::vector<PhotoWidget *> photo_widgets_;
  std::vector<QLabel *> photo_text_widgets_;
  QLabel *title_text_widget_ = nullptr;
  QSvgWidget *background_svg_ = nullptr;
  TextButton *close_ = nullptr;
  TextButton *delete_ = nullptr;
};

} // namespace FirstYear::UI
#endif // FIRSTYEAR_UI_DEFAULT_FRAME_WIDGET_H
