#include "mainwindow.h"
#include <stdafx.h>

static const char *c_org_str = "natshch";
static const char *c_app_str = "FirstYear";
static const char *c_log_str = "/logs/FirstYearLog.txt";
static const char *c_logger_str = "logger";

void initLogger() {
  try {
    QString logs_path = QDir::currentPath() + c_log_str;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        QDir().toNativeSeparators(logs_path).toStdString(), 1024 * 1024, 5,
        false));
    auto combined_logger = std::make_shared<spdlog::logger>(
        c_logger_str, begin(sinks), end(sinks));
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);
    spdlog::info("Log file path {}", logs_path.toStdString());
    // spdlog::set_default_logger(logger);
  } catch (const spdlog::spdlog_ex &ex) {
#ifndef Q_OS_IOS
    // TODO fix iOS exception
    UNI_ASSERT(false);
#endif
  }
}

int main(int argc, char *argv[]) {
  initLogger();
  spdlog::info("Initialize First Year application");

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName(c_org_str);
  QCoreApplication::setApplicationName(c_app_str);
  MainWindow w;
  w.show();

  auto res = a.exec();

  spdlog::info("Application closed");
  spdlog::drop_all();
  return res;
}
