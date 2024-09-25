#include <cstddef>
#include <limits>
#include <memory>
#include <optional>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#ifdef Q_OS_ANDROID
#include <spdlog/sinks/android_sink.h>
#endif
#include <uniassert/uniassert.h>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGestureEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QImageReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPixmap>
#include <QPolygon>
#include <QPushButton>
// #include <QRandomGenerator>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QTouchEvent>
#include <QVariant>
#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <jni.h>
#endif

#include <QSwipeView>

#include <Core/FrameControl/FrameControl.h>
#include <Core/Project/Project.h>
#include <Types.h>
#include <UI/FrameWidgets/DefaultFrameWidget.h>
#include <UI/FrameWidgets/Photo.h>
