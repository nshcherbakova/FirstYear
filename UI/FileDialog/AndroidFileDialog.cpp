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

#include "AndroidFileDialog.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QtCore/QJniObject>

namespace FirstYear::UI::Android {

#define RESULT_OK -1
#define REQUEST_CODE 1305 // Arbitrary

const char JniIntentClass[] = "android/content/Intent";

void AndroidFileDialog::handleActivityResult(int requestCode, int resultCode,
                                             const QJniObject &intent) {
  if (requestCode != REQUEST_CODE)
    return;

  if (resultCode != RESULT_OK) {
    emit SignalFileSelectionRejected();
    hide();
    return;
  }

  const QJniObject uri =
      intent.callObjectMethod("getData", "()Landroid/net/Uri;");
  if (uri.isValid()) {
    takePersistableUriPermission(uri);
    selected_files_.append((uri.toString()));
    emit SignalFileSelected(selected_files_.first());
    hide();
    return;
  }

  const QJniObject uriClipData =
      intent.callObjectMethod("getClipData", "()Landroid/content/ClipData;");
  if (uriClipData.isValid()) {
    const int size = uriClipData.callMethod<jint>("getItemCount");
    for (int i = 0; i < size; ++i) {
      QJniObject item = uriClipData.callObjectMethod(
          "getItemAt", "(I)Landroid/content/ClipData$Item;", i);

      QJniObject itemUri =
          item.callObjectMethod("getUri", "()Landroid/net/Uri;");
      takePersistableUriPermission(itemUri);
      selected_files_.append(itemUri.toString());
    }
    emit SignalFilesSelected(selected_files_);
  }
  hide();
  return;
}

void AndroidFileDialog::takePersistableUriPermission(const QJniObject &uri) {
  int modeFlags = QJniObject::getStaticField<jint>(
      JniIntentClass, "FLAG_GRANT_READ_URI_PERMISSION");

  QJniObject activity = QNativeInterface::QAndroidApplication::context();

  QJniObject contentResolver = activity.callObjectMethod(
      "getContentResolver", "()Landroid/content/ContentResolver;");
  contentResolver.callMethod<void>("takePersistableUriPermission",
                                   "(Landroid/net/Uri;I)V", uri.object(),
                                   modeFlags);
}

bool AndroidFileDialog::show(bool multiple_files) {

  const QJniObject ACTION_OPEN_DOCUMENT = QJniObject::getStaticObjectField(
      JniIntentClass, "ACTION_GET_CONTENT", "Ljava/lang/String;");

  const QJniObject ALLOW_MULTIPLE =
      QJniObject::fromString("android.intent.extra.ALLOW_MULTIPLE");

  QJniObject intent("android/content/Intent");
  if (ACTION_OPEN_DOCUMENT.isValid() && intent.isValid()) {
    intent.callObjectMethod("setAction",
                            "(Ljava/lang/String;)Landroid/content/Intent;",
                            ACTION_OPEN_DOCUMENT.object<jstring>());
    intent.callObjectMethod(
        "setType", "(Ljava/lang/String;)Landroid/content/Intent;",
        QJniObject::fromString("image/*").object<jstring>());

    // intent.callObjectMethod(
    //    "setType", "(Ljava/lang/String;)Landroid/content/Intent;",
    //      QJniObject::fromString("image/png").object<jstring>());

    intent.callObjectMethod(
        "putExtra", "(Ljava/lang/String;Z)Landroid/content/Intent;",
        ALLOW_MULTIPLE.object<jstring>(), jboolean(multiple_files));

    QtAndroidPrivate::startActivity(intent.object<jobject>(), REQUEST_CODE,
                                    this);
  }
  return true;
}

void AndroidFileDialog::hide() {
  if (m_eventLoop.isRunning())
    m_eventLoop.exit();
}

void AndroidFileDialog::exec() { m_eventLoop.exec(QEventLoop::DialogExec); }

QList<QString> AndroidFileDialog::selectedFiles() {
  auto filse = selected_files_;
  selected_files_.clear();
  return filse;
}

QString AndroidFileDialog::getOpenFileName() {
  AndroidFileDialog dialog;

  dialog.show(false);
  dialog.exec();
  const auto files = dialog.selectedFiles();
  return !files.empty() ? files.front() : QString();
}

QList<QString> AndroidFileDialog::getOpenFileNames() {
  AndroidFileDialog dialog;

  dialog.show(true);
  dialog.exec();
  return dialog.selectedFiles();
}

void AndroidFileDialog::showFileDialog() { show(false); }

void AndroidFileDialog::showFilesDialog() { show(true); }

} // namespace FirstYear::UI::Android
// QT_END_NAMESPACE
