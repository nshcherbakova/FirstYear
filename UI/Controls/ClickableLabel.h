#pragma once
#ifndef FE_CLICKABLE_LABEL_H
#define FE_CLICKABLE_LABEL_H

#include <QFont>
#include <QLabel>
#include <QString>

namespace FirstYear::UI {

class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget *parent, int font_size, QString font_color,
                          QString font_family, bool hide_edit_icon = false,
                          bool ajust_text = false);

  void setText(QString text);
  void setFontSize(int size);

  QString text() const;

signals:
  void clicked();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event) override final;
  virtual void resizeEvent(QResizeEvent *event) override final;

private:
  void updateFontSize();

private:
  QFont font_;
  QString styled_text_;
  QString icon_text_;
  QString text_;
  int icon_size_ = 10;
  bool hide_edit_icon_ = false;
  bool ajust_text_ = false;
};

} // namespace FirstYear::UI
#endif // FE_SHARE_BUTTON_H
