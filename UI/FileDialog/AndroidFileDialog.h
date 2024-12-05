/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB)
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
