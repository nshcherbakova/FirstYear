#include "mainwindow.h"
#include <stdafx.h>

static const char *c_org_str = "natshch";
static const char *c_app_str = "FirstYear";
static const char *c_log_str = "/logs/FirstYearLog.txt";
static const char *c_logger_str = "logger";
static const char *c_fonts_dir_str = ":/fonts";
static const char *c_frames_dir_str = ":/frames";

void initLogger() {
  try {
    QString logs_path =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        c_log_str;

#ifdef Q_OS_ANDROID
    auto console_logger = std::make_shared<spdlog::sinks::android_sink_st>();
#else
    auto console_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
#endif
    console_logger->set_level(spdlog::level::debug);

    auto file_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        QDir().toNativeSeparators(logs_path).toStdString(), 1024 * 1024, 5,
        false);
    file_logger->set_level(spdlog::level::err);

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_logger);
    sinks.push_back(file_logger);
    auto combined_logger = std::make_shared<spdlog::logger>(
        c_logger_str, begin(sinks), end(sinks));

    spdlog::set_default_logger(combined_logger);
    spdlog::info("Log file path {}", logs_path.toStdString());

  } catch (const spdlog::spdlog_ex &ex) {
#ifndef Q_OS_IOS
    // TODO fix iOS exception
    // UNI_ASSERT(false);
#endif
  }
}

#ifdef Q_OS_ANDROID
bool requestStoragePermission() {
  const QVector<QString> permissions(
      {"android.permission.WRITE_EXTERNAL_STORAGE",
       "android.permission.READ_EXTERNAL_STORAGE",
       "android.permission.MANAGE_EXTERNAL_STORAGE"});

  for (const QString &permission : permissions) {
    auto result = QtAndroidPrivate::checkPermission(permission);
    if (result.result() == QtAndroidPrivate::PermissionResult::Denied) {
      QFuture<QtAndroidPrivate::PermissionResult> resultHash =
          QtAndroidPrivate::requestPermission(permission);
      if (resultHash.result() == QtAndroidPrivate::PermissionResult::Denied)
        return false;
    }
  }

  return true;
}

void accessAllFiles() {
  if (QOperatingSystemVersion::current() <
      QOperatingSystemVersion(QOperatingSystemVersion::Android, 11)) {
    qDebug()
        << "it is less then Android 11 - ALL FILES permission isn't possible!";
    return;
  }
  // Here you have to set your PackageName

  jboolean value = QJniObject::callStaticMethod<jboolean>(
      "android/os/Environment", "isExternalStorageManager");
  if (value == false) {
    qDebug() << "requesting ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION";

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject packageManager = activity.callObjectMethod(
        "getPackageManager", "()Landroid/content/pm/PackageManager;");
    QJniObject packageName =
        activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");

    QJniObject ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION =
        QJniObject::getStaticObjectField(
            "android/provider/Settings",
            "ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION",
            "Ljava/lang/String;");
    QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
                      ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION.object());
    QJniObject jniPath =
        QJniObject::fromString("package:" + packageName.toString());
    QJniObject jniUri = QJniObject::callStaticObjectMethod(
        "android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;",
        jniPath.object<jstring>());
    QJniObject jniResult = intent.callObjectMethod(
        "setData", "(Landroid/net/Uri;)Landroid/content/Intent;",
        jniUri.object<jobject>());
    QtAndroidPrivate::startActivity(intent, 0);
  } else {
    qDebug() << "SUCCESS ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION";
  }
}
void setScreenOrientation() {
  QJniObject activity = QNativeInterface::QAndroidApplication::context();
  if (activity.isValid()) {
    int orientation = QJniObject::getStaticField<int>(
        "android.content.pm.ActivityInfo", "SCREEN_ORIENTATION_SENSOR");
    activity.callMethod<void>("setRequestedOrientation" // method name
                              ,
                              "(I)V" // signature
                              ,
                              orientation);
  }
}

#endif

QStringList collectFrames() {

  return {
      "rainbow", "1",      "giraffe", "2",       "Girl",    "shapes",  "Bunny",
      "stars",   "Light2", "colors",  "Pattern", "baloons", "Flowers",
  };

  QStringList frames;
  QDirIterator it(c_frames_dir_str);

  while (it.hasNext()) {
    const auto fn = it.nextFileInfo();
    qDebug() << fn.fileName();
    frames << fn.fileName();
  }
  return frames;
}

void loadFonts() {
  QDirIterator it(c_fonts_dir_str);
  while (it.hasNext()) {
    const QString fn = it.nextFileInfo().fileName();
    QFontDatabase::addApplicationFont(QString(c_fonts_dir_str) + "/" + fn);
    qDebug() << "Font file: " << fn;
  }
}

int main(int argc, char *argv[]) {

#ifdef Q_OS_ANDROID

  /*  requestStoragePermission();
    accessAllFiles();
    setScreenOrientation();*/
#endif

  initLogger();
  spdlog::info("Initialize First Year application");

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName(c_org_str);
  QCoreApplication::setApplicationName(c_app_str);
  QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
  QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);

  loadFonts();

  FirstYear::Core::FrameControl frame_control(&a);
  frame_control.LoadProject();

  const auto frames = collectFrames();

  FirstYear::UI::MainWindow w(frame_control, frames);
  // todo list of farmes and registration system

  w.show();

  auto res = a.exec();

  frame_control.SaveProject();
  spdlog::info("Application closed");

  return res;
}
