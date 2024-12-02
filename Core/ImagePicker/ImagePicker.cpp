/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "ImagePicker.h"

#include <QImage>
#include <QtCore/private/qandroidextras_p.h>

const int REQUEST_CODE = 123;
const jint RESULT_OK =
    QJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");

namespace FirstYear::Core::Android {
void ImagesPicker::show() {
  QJniObject ACTION_PICK =
      QJniObject::fromString("android.intent.action.GET_CONTENT");
  QJniObject ALLOW_MULTIPLE =
      QJniObject::fromString("android.intent.extra.ALLOW_MULTIPLE");
  /*  QJniObject PICK_IMAGES_MAX =
        QJniObject::fromString("android.provider.extra.PICK_IMAGES_MAX");

    QJniObject INITIAL_URI =
        QJniObject::fromString("android.provider.extra.INITIAL_URI");

    QJniObject test =
        QJniObject::fromString("content://com.android.providers.media.documents/document/image%3A1000015060");
  */

  QJniObject intent("android/content/Intent");
  if (ACTION_PICK.isValid() && intent.isValid()) {
    intent.callObjectMethod("setAction",
                            "(Ljava/lang/String;)Landroid/content/Intent;",
                            ACTION_PICK.object<jstring>());
    intent.callObjectMethod(
        "setType", "(Ljava/lang/String;)Landroid/content/Intent;",
        QJniObject::fromString("image/*").object<jstring>());
    intent.callObjectMethod("putExtra",
                            "(Ljava/lang/String;Z)Landroid/content/Intent;",
                            ALLOW_MULTIPLE.object<jstring>(), jboolean(true));

    //  intent.callObjectMethod("putExtra",
    //                           "(Ljava/lang/String;Ljava/lang/String)Landroid/content/Intent;",
    //                           INITIAL_URI.object<jstring>(),
    //                           test.object<jstring>());

    QtAndroidPrivate::startActivity(intent.object<jobject>(), REQUEST_CODE,
                                    this);
  }
}

void ImagesPicker::handleActivityResult(int receiverRequestCode, int resultCode,
                                        const QJniObject &data) {

  //  qDebug() << "handleActivityResult " << receiverRequestCode << " " <<
  //  resultCode;
  //    qDebug() << "RESULT_OK " << RESULT_OK;
  if (receiverRequestCode == REQUEST_CODE && resultCode == RESULT_OK) {
    QJniObject resultArray = QJniObject::callStaticObjectMethod(
        "org/nshchapps/firstyear/utils/Converter", "UriListToStringArray",
        "(Landroid/content/Intent;)[Ljava/lang/String;",
        data.object<jobject>());

    if (resultArray != nullptr) {
      QJniEnvironment env;
      int count = env->GetArrayLength((jobjectArray)resultArray.object());

      QStringList files;
      for (int i = 0; i < count; i++) {
        QJniObject item = (jstring)env->GetObjectArrayElement(
            (jobjectArray)resultArray.object(), i);

        files << item.toString();
        qDebug() << "2 handleActivityResult " << item.toString();
      }

      emit SignalPickedImages(files);
    }
  }
}

void SingleImagePicker::show() {
  QJniObject ACTION_PICK =
      QJniObject::fromString("android.intent.action.GET_CONTENT");

  QJniObject intent("android/content/Intent");
  if (ACTION_PICK.isValid() && intent.isValid()) {
    intent.callObjectMethod("setAction",
                            "(Ljava/lang/String;)Landroid/content/Intent;",
                            ACTION_PICK.object<jstring>());
    intent.callObjectMethod(
        "setType", "(Ljava/lang/String;)Landroid/content/Intent;",
        QJniObject::fromString("image/*").object<jstring>());

    QtAndroidPrivate::startActivity(intent.object<jobject>(), REQUEST_CODE,
                                    this);
  }
}

void SingleImagePicker::handleActivityResult(int receiverRequestCode,
                                             int resultCode,
                                             const QJniObject &data) {

  if (receiverRequestCode == REQUEST_CODE && resultCode == RESULT_OK) {
    QJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
    qDebug() << "1 handleActivityResult " << uri.toString();
    emit SignalPickedImage(uri.toString());
  }
}
} // namespace FirstYear::Core::Android
