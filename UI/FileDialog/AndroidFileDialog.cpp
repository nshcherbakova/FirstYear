#include "AndroidFileDialog.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include <QtCore/QJniObject>

namespace FirstYear::UI::Android {

#define RESULT_OK -1
#define REQUEST_CODE 1305 // Arbitrary

const char JniIntentClass[] = "android/content/Intent";

static QString getRealPathFromUri(const QUrl &url) {
  QString path = "";
  QString localfile = url.toLocalFile();
  if ((QFileInfo(localfile).isFile() || QFileInfo(localfile).isDir()) &&
      localfile != url.toString()) {
    return localfile;
  }

  QJniObject jUrl = QJniObject::fromString(url.toString());
  QJniObject jContext = QtAndroidPrivate::context();
  QJniObject jContentResolver = jContext.callObjectMethod(
      "getContentResolver", "()Landroid/content/ContentResolver;");
  QJniObject jUri = QJniObject::callStaticObjectMethod(
      "android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;",
      jUrl.object<jstring>());
  QJniObject jCursor = jContentResolver.callObjectMethod(
      "query",
      "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/"
      "String;Ljava/lang/String;)Landroid/database/Cursor;",
      jUri.object<jobject>(), nullptr, nullptr, nullptr, nullptr);
  QJniObject jScheme =
      jUri.callObjectMethod("getScheme", "()Ljava/lang/String;");
  QJniObject authority;
  if (jScheme.isValid()) {
    authority = jUri.callObjectMethod("getAuthority", "()Ljava/lang/String;");
  }
  if (authority.isValid() &&
      authority.toString() == "com.android.externalstorage.documents") {
    QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
    path = jPath.toString();
  } else if (jCursor.isValid() && jCursor.callMethod<jboolean>("moveToFirst")) {
    QJniObject jColumnIndex = QJniObject::fromString("_data");
    jint columnIndex = jCursor.callMethod<jint>("getColumnIndexOrThrow",
                                                "(Ljava/lang/String;)I",
                                                jColumnIndex.object<jstring>());
    QJniObject jRealPath = jCursor.callObjectMethod(
        "getString", "(I)Ljava/lang/String;", columnIndex);
    path = jRealPath.toString();
    if (authority.isValid() &&
        authority.toString().startsWith("com.android.providers") &&
        !url.toString().startsWith("content://media/external/")) {
      QStringList list = path.split(":");
      if (list.count() == 2) {
        QString type = list.at(0);
        QString id = list.at(1);
        if (type == "image")
          type = type + "s";
        if (type == "document" || type == "documents")
          type = "file";
        if (type == "msf")
          type = "downloads";
        if (QList<QString>({"images", "video", "audio"}).contains(type))
          type = type + "/media";
        path = "content://media/external/" + type;
        path = path + "/" + id;
        return getRealPathFromUri(path);
      }
    }
  } else {
    QJniObject jPath = jUri.callObjectMethod("getPath", "()Ljava/lang/String;");
    path = jPath.toString();
    qDebug() << QFile::exists(path) << path;
  }

  if (path.startsWith("primary:")) {
    path = path.remove(0, QString("primary:").length());
    path = "/sdcard/" + path;
  } else if (path.startsWith("/document/primary:")) {
    path = path.remove(0, QString("/document/primary:").length());
    path = "/sdcard/" + path;
  } else if (path.startsWith("/tree/primary:")) {
    path = path.remove(0, QString("/tree/primary:").length());
    path = "/sdcard/" + path;
  } else if (path.startsWith("/storage/emulated/0/")) {
    path = path.remove(0, QString("/storage/emulated/0/").length());
    path = "/sdcard/" + path;
  } else if (path.startsWith("/tree//")) {
    path = path.remove(0, QString("/tree//").length());
    path = "/" + path;
  }
  if (!QFileInfo(path).isFile() && !QFileInfo(path).isDir() &&
      !path.startsWith("/data"))
    return url.toString();
  return path;
}

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
    selected_files_.append((getRealPathFromUri(uri.toString())));
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
      selected_files_.append(getRealPathFromUri(itemUri.toString()));
    }
    emit SignalFilesSelected(selected_files_);
  }
  hide();
  return;
}

void AndroidFileDialog::takePersistableUriPermission(const QJniObject &uri) {
  int modeFlags = QJniObject::getStaticField<jint>(
      JniIntentClass, "FLAG_GRANT_READ_URI_PERMISSION");

  QJniObject jContext = QtAndroidPrivate::context();
  QJniObject contentResolver = jContext.callObjectMethod(
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
