#include "AndroidShareUtils.h"
#include <stdafx.h>
// (c) 2017 Ekkehard Gentz (ekke) @ekkescorner
// my blog about Qt for mobile: http://j.mp/qt-x
// see also /COPYRIGHT and /LICENSE

namespace {

const QString kFileProviderPath = "/shared_files";

} // namespace

namespace ShareUtils::Android {
AndroidShareUtils *AndroidShareUtils::mInstance = nullptr;

AndroidShareUtils::AndroidShareUtils(QObject *parent)
    : IPlatformShareUtils(parent) {
  // create a save directory, if it doesn't already exist
  mSavePath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
      kFileProviderPath + "/";
  QDir tempDir(mSavePath);
  if (!tempDir.exists()) {
    QDir("").mkpath(mSavePath);
  }

  /* QJniObject::callStaticMethod<void>("org/nshchapps/firstyear/utils/QShareUtils",
   "setActivity", "(Landroid/app/Activity;)V",
   QNativeInterface::QAndroidApplication::context().object());*/

  // we need the instance for JNI Call
  mInstance = this;
}

AndroidShareUtils *AndroidShareUtils::getInstance() {
  if (!mInstance) {
    mInstance = new AndroidShareUtils;
  }

  return mInstance;
}

bool AndroidShareUtils::checkMimeTypeView(const QString &mimeType) {
  QJniObject jsMime = QJniObject::fromString(mimeType);
  jboolean verified = QJniObject::callStaticMethod<jboolean>(
      "org/nshchapps/firstyear/utils/QShareUtils", "checkMimeTypeView",
      "(Ljava/lang/String;)Z", jsMime.object<jstring>());
  qDebug() << Q_FUNC_INFO << __LINE__ << verified;
  return verified;
}

/*
 * If a requestId was set we want to get the Activity Result back (recommended)
 * We need the Request Id and Result Id to control our workflow
 */
void AndroidShareUtils::sendFile(const QString &filePath, const QString &title,
                                 const QString &mimeType, int requestId) {
  QFileInfo fileInfo(filePath);
  QFileInfo saveDirInfo(mSavePath);

  // first, do a sanity check that the file being sent actually exists.
  if (!fileInfo.exists()) {
    qWarning() << "Attempting to share a file that doesn't exist at path: "
               << filePath;
    return;
  }

  // next, check if the file is in the sharing directory. If not, add it.
  auto path = filePath;
  if (fileInfo.dir() != saveDirInfo.dir()) {
    path = mSavePath + fileInfo.fileName();
    if (QFile::exists(path)) {
      QFile::remove(path);
    }
    auto result = QFile::copy(filePath, path);
    if (!result) {
      qWarning() << " could not save file to " << path;
      return;
    }
  }

  // now, send the file
  QJniObject jsPath = QJniObject::fromString(path);
  QJniObject jsTitle = QJniObject::fromString(title);
  QJniObject jsMimeType = QJniObject::fromString(mimeType);
  jboolean ok = QJniObject::callStaticMethod<jboolean>(
      "org/nshchapps/firstyear/utils/QShareUtils", "sendFile",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)Z",
      jsPath.object<jstring>(), jsTitle.object<jstring>(),
      jsMimeType.object<jstring>(), requestId);
  if (!ok) {
    qWarning() << "Unable to resolve activity from Java";
    emit shareNoAppAvailable(requestId);
  }
}

// used from QAndroidActivityResultReceiver
void AndroidShareUtils::handleActivityResult(int receiverRequestCode,
                                             int resultCode,
                                             const QJniObject &) {
  // qDebug() << "From JNI QAndroidActivityResultReceiver: " <<
  // receiverRequestCode
  //             << "ResultCode:" << resultCode;
  processActivityResult(receiverRequestCode, resultCode);
}

// used from Activity.java onActivityResult()
void AndroidShareUtils::onActivityResult(int requestCode, int resultCode) {
  //    qDebug() << "From Java Activity onActivityResult: " << requestCode
  //             << "ResultCode:" << resultCode;
  processActivityResult(requestCode, resultCode);
}

void AndroidShareUtils::processActivityResult(int requestCode, int resultCode) {
  const int RESULT_OK = -1;
  const int RESULT_CANCELED = 0;

  // we're getting RESULT_OK only if edit is done
  if (resultCode == RESULT_OK) {
    // emit shareEditDone(requestCode);
  } else if (resultCode == RESULT_CANCELED) {
    emit shareFinished(requestCode);
  } else {
    qDebug() << "wrong result code: " << resultCode
             << " from request: " << requestCode;
    emit shareError(requestCode, tr("Share: an Error occured"));
  }
}

void AndroidShareUtils::checkPendingIntents() {
  QJniObject activity = QtAndroidPrivate::activity();
  if (activity.isValid()) {
    // create a Java String for the Working Dir Path
    QJniObject jniWorkingDir = QJniObject::fromString(mSavePath);
    if (!jniWorkingDir.isValid()) {
      qWarning() << "QJniObject jniWorkingDir not valid.";
      emit shareError(0, tr("Share: an Error occured\nWorkingDir not valid"));
      return;
    }
    activity.callMethod<void>("checkPendingIntents", "(Ljava/lang/String;)V",
                              jniWorkingDir.object<jstring>());
    qDebug() << "checkPendingIntents: " << mSavePath;
    return;
  }
  qDebug() << "checkPendingIntents: Activity not valid";
}

void AndroidShareUtils::clearTempDir() {
  QDir saveDir(mSavePath);
  saveDir.setNameFilters(QStringList() << "*.*");
  saveDir.setFilter(QDir::Files);
  for (const auto &dirFile : saveDir.entryList()) {
    saveDir.remove(dirFile);
  }
}

void AndroidShareUtils::setFileUrlReceived(const QString &url) {
  if (url.isEmpty()) {
    qWarning() << "setFileUrlReceived: we got an empty URL";
    emit shareError(0, tr("Empty URL received"));
    return;
  }
  // qDebug() << "AndroidShareUtils setFileUrlReceived: we got the File URL from
  // JAVA: " << url;
  QString myUrl;
  if (url.startsWith("file://")) {
    myUrl = url.right(url.length() - 7);
  } else {
    myUrl = url;
  }

  // check if File exists
  QFileInfo fileInfo = QFileInfo(myUrl);
  if (fileInfo.exists()) {
    emit fileUrlReceived(myUrl);
  } else {
    emit shareError(0, tr("File does not exist: %1").arg(myUrl));
  }
}

// to be safe we check if a File Url from java really exists for Qt
// if not on the Java side we'll try to read the content as Stream
bool AndroidShareUtils::checkFileExits(const QString &url) {
  if (url.isEmpty()) {
    emit shareError(0, tr("Empty URL received"));
    return false;
  }
  // qDebug() << "AndroidShareUtils checkFileExits: we got the File URL from
  // JAVA: " << url;
  QString myUrl;
  if (url.startsWith("file://")) {
    myUrl = url.right(url.length() - 7);
  } else {
    myUrl = url;
  }

  // check if File exists
  QFileInfo fileInfo = QFileInfo(myUrl);
  return fileInfo.exists();
}
} // namespace ShareUtils::Android

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_nshchapps_firstyear_activity_QShareActivity_setFileUrlReceived(
    JNIEnv *env, jobject, jstring url) {
  const char *urlStr = env->GetStringUTFChars(url, nullptr);
  ShareUtils::Android::AndroidShareUtils::getInstance()->setFileUrlReceived(
      urlStr);
  env->ReleaseStringUTFChars(url, urlStr);
  return;
}

JNIEXPORT bool JNICALL
Java_org_nshchapps_firstyear_activity_QShareActivity_checkFileExits(
    JNIEnv *env, jobject, jstring url) {
  const char *urlStr = env->GetStringUTFChars(url, nullptr);
  bool exists =
      ShareUtils::Android::AndroidShareUtils::getInstance()->checkFileExits(
          urlStr);
  env->ReleaseStringUTFChars(url, urlStr);
  return exists;
}

JNIEXPORT void JNICALL
Java_org_nshchapps_firstyear_activity_QShareActivity_fireActivityResult(
    JNIEnv *, jobject, jint requestCode, jint resultCode) {
  ShareUtils::Android::AndroidShareUtils::getInstance()->onActivityResult(
      requestCode, resultCode);
  return;
}

#ifdef __cplusplus
}
#endif
