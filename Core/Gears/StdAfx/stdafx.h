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
#include <QLineEdit>
#include <QPixmap>
#include <QPolygon>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringList>
#include <QSvgRenderer>
#include <QSvgWidget>
// #include <QTextEdit>
#include <QTouchEvent>
#include <QVariant>
#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <UI/FileDialog/AndroidFileDialog.h>
#include <jni.h>
#endif

#include <Core/FrameControl/FrameControl.h>
#include <Core/Gears/Json/Json.h>
#include <Core/Project/Project.h>
#include <Core/shareutils/ShareUtilsCpp.h>
#include <Types.h>
#include <UI/Controls/Buttons.h>
#include <UI/Controls/ClickableLabel.h>
#include <UI/Controls/LineEdit.h>
#include <UI/FileDialog/FileDialog.h>
#include <UI/FrameWidgets/DefaultTemplateWidget.h>
#include <UI/FrameWidgets/Photo.h>
#include <UI/FrameWidgets/TouchButton.h>
#include <UI/Styles/Styles.h>
#include <UI/SwipeView/SwipeWidgetsList.hpp>
