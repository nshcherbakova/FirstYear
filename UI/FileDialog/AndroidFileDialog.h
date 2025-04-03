#ifndef FE_AFILEDIALOG_H
#define FE_AFILEDIALOG_H

#include <jni.h>

#include <QEventLoop>
#include <QtCore/QJniObject>
#include <QtCore/private/qandroidextras_p.h>

namespace FirstYear::UI::Android {

class AndroidFileDialog : public QObject,
                          public QAndroidActivityResultReceiver {
  Q_OBJECT

public:
  static QString getOpenFileName();
  static QList<QString> getOpenFileNames();

  void showFileDialog();
  void showFilesDialog();

  virtual void handleActivityResult(int receiverRequestCode, int resultCode,
                                    const QJniObject &data) override;

signals:
  void SignalFileSelected(QString file);
  void SignalFilesSelected(QList<QString>);
  void SignalFileSelectionRejected();

private:
  void exec();
  bool show(bool multiple_files);
  void hide();

  void takePersistableUriPermission(const QJniObject &uri);
  QList<QString> selectedFiles();

private:
  QEventLoop m_eventLoop;
  QList<QString> selected_files_;
};

} // namespace FirstYear::UI::Android

#endif // FE_FILEDIALOG_H
