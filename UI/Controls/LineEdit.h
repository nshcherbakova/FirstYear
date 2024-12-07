#pragma once
#ifndef FE_LINE_EDIT_H
#define FE_LINE_EDIT_H

#include <QWidget>

class QLineEdit;

namespace FirstYear::UI {

class LineEditWidget final : public QWidget {

  Q_OBJECT
public:
  LineEditWidget(QWidget *parent);

public:
  virtual void setVisible(bool visible) override final;
  void setText(QString text, int id);

signals:
  void SignalTextChanged(QString text, int id);

protected:
  virtual void resizeEvent(QResizeEvent *) override final;
  virtual void mouseReleaseEvent(QMouseEvent *) override final;

private:
  QLineEdit *line_edit_ = nullptr;
  int id_ = -1;
};

} // namespace FirstYear::UI
#endif // FE_LINE_EDIT_H
