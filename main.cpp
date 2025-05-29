#include "mainwindow.h"
#include <stdafx.h>

static const char *c_org_str = "natshch";
static const char *c_app_str = "FirstYear";
static const char *c_log_dir_str = "/logs/";
static const char *c_log_file_str = "FirstYearLog.txt";
static const char *c_logger_str = "logger";
static const char *c_fonts_dir_str = ":/fonts";
static const char *c_dev_settings_file_str = "DeveloperSettings.ini";

QString initLogger() {
  try {

#ifdef Q_OS_ANDROID
    auto console_logger = std::make_shared<spdlog::sinks::android_sink_st>();
#else
    auto console_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
#endif
    console_logger->set_level(spdlog::level::debug);

    const QString logs_path =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        c_log_dir_str;
    if (!QDir().exists(logs_path)) {
      QDir().mkpath(logs_path);
    }
    const QString log_file_path = logs_path + c_log_file_str;

    auto file_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        QDir().toNativeSeparators(log_file_path).toStdString(), 1024 * 1024, 5,
        false);
    file_logger->set_level(spdlog::level::err);

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_logger);
    sinks.push_back(file_logger);
    auto combined_logger = std::make_shared<spdlog::logger>(
        c_logger_str, begin(sinks), end(sinks));

    spdlog::set_default_logger(combined_logger);
    spdlog::info("Log file path {}", log_file_path.toStdString());

    return log_file_path;
  } catch (const spdlog::spdlog_ex &ex) {
#ifndef Q_OS_IOS
    // TODO fix iOS exception
    // UNI_ASSERT(false);
#endif
  }
  return QString();
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

QString loadLocale() {
  const auto settings_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" +
      c_dev_settings_file_str;
  /*{
  QSettings developer_settings1(settings_path, QSettings::IniFormat);
  developer_settings1.setValue("locale", "ru");
  }*/
  if (QFileInfo::exists(settings_path)) {
    QSettings developer_settings(settings_path, QSettings::IniFormat);
    const auto locale = developer_settings.value("locale");

    if (locale.isValid()) {
      return locale.toString();
    }
  }
  return QLocale::languageToCode(QLocale::system().language(),
                                 QLocale::ISO639Alpha2);
}

QStringList collectFrames() {

  const QString list =
      QObject::tr("rainbow,1,giraffe,2,Girl,shapes,Bunny,stars,Light2,colors,"
                  "Pattern,baloons,Flowers");
  return list.split(",");

  /*QStringList frames;
  QDirIterator it(c_frames_dir_str);

  while (it.hasNext()) {
    const auto fn = it.nextFileInfo();
    qDebug() << fn.fileName();
    frames << fn.fileName();
  }
  return frames;*/
}

void loadFonts(const QString &locale) {

  QString fonts_dir = QString(c_fonts_dir_str) + "/" + locale;

  if (QDir(fonts_dir).isEmpty()) {
    fonts_dir = c_fonts_dir_str;
  }
  QDirIterator it(fonts_dir);
  while (it.hasNext()) {
    const auto &file_info = it.nextFileInfo();

    if (!file_info.isFile() || !file_info.isReadable()) {
      continue;
    }

    const QString fn = file_info.fileName();
    QFontDatabase::addApplicationFont(QString(fonts_dir) + "/" + fn);
    qDebug() << "Font file: " << fn;
  }
}

void localization(QApplication &application, QTranslator &translator,
                  const QString &locale) {

  qDebug() << "language: " << locale;

  const auto translation_file_name =
      QString(":/translations/translation_") + locale;
  if (!translator.load(translation_file_name)) {
    spdlog::info("Can't load translation file {} for locale {}",
                 translation_file_name.toStdString(), locale.toStdString());
    return;
  }
  application.installTranslator(&translator);
}

int main(int argc, char *argv[]) {

#ifdef Q_OS_ANDROID

  /*  requestStoragePermission();
    accessAllFiles();
    setScreenOrientation();*/
#endif

  QCoreApplication::setOrganizationName(c_org_str);
  QCoreApplication::setApplicationName(c_app_str);

  const QString log_file_path = initLogger();
  spdlog::info("Initialize First Year application");

  QApplication a(argc, argv);
  QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
  QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);

  const auto locale = loadLocale();

  QTranslator translator;
  localization(a, translator, locale);

  FirstYear::Core::FrameControl frame_control({&a, log_file_path, locale});

  loadFonts(locale);

  frame_control.LoadProject();

  const auto frames = collectFrames();

  FirstYear::UI::MainWindow w(frame_control, frames);

  w.show();

  auto res = a.exec();

  frame_control.SaveProject();
  spdlog::info("Application closed");

  return res;
}
