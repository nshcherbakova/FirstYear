QT       += core gui
# QT = core gui widgets openglwidgets opengl network svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
android {
   QT += core-private
}
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PROJECT_PATH = $$PWD
PROJECT_THIRDPARTY = /Users/nshcherbakova/Documents/dev/Thirdparty

INCLUDEPATH +=$$PROJECT_PATH/Core/Gears/StdAfx/ \
$$PROJECT_PATH/UI/QSwipeView/ \
    #$$PROJECT_PATH \
    $$PROJECT_THIRDPARTY/uniassert/include/ \
    $$PROJECT_THIRDPARTY/spdlog/include/

PRECOMPILED_HEADER = Core/Gears/StdAfx/stdafx.h

SOURCES += \
    Core/FrameControl/FrameControl.cpp \
    Core/Gears/StdAfx/stdafx.cpp \
    Core/Project/FileSystemProjectLoader.cpp \
    Core/Project/FileSystemProjectWriter.cpp \
    Core/Project/Project.cpp \
    Core/shareutils/AndroidShareUtils.cpp \
    Core/shareutils/DummyShareUtils.cpp \
    Core/shareutils/ShareUtilsCpp.cpp \
    UI/FrameWidgets/DefaultTemplateWidget.cpp \
    UI/FrameWidgets/ImageButton.cpp \
    UI/FrameWidgets/PhotoTuneWidget.cpp \
    UI/FrameWidgets/PhotoWidget.cpp \
    UI/FrameWidgets/PreviewWidget.cpp \
    UI/FrameWidgets/TouchButton.cpp \
    UI/SwipeView/SwipeWidgetsList.cpp \
    UI/Utility.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Core/FrameControl/FrameControl.h \
    Core/Gears/StdAfx/stdafx.h \
    Core/Project/Constants.h \
    Core/Project/FileSystemProjectLoader.h \
    Core/Project/FileSystemProjectWriter.h \
    Core/Project/IProjecLoader.h \
    Core/Project/Project.h \
    Core/shareutils/AndroidShareUtils.h \
    Core/shareutils/DummyShareUtils.h \
    Core/shareutils/IPlatformShareUtils.h \
    Core/shareutils/ShareUtilsCpp.h \
    UI/FrameWidgets/DefaultTemplateWidget.h \
    UI/FrameWidgets/ImageButton.h \
    UI/FrameWidgets/Photo.h \
    UI/FrameWidgets/PhotoTuneWidget.h \
    UI/FrameWidgets/PhotoWidget.h \
    UI/FrameWidgets/PreviewWidget.h \
    UI/FrameWidgets/TouchButton.h \
    UI/SwipeView/SwipeWidgetsList.hpp \
    UI/Utility.h \
    mainwindow.h

!android {
HEADERS -= Core/shareutils/AndroidShareUtils.h
SOURCES -= Core/shareutils/AndroidShareUtils.cpp
}

QSwipeView/qpageindicator.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    resource.qrc

android {
DISTFILES += \
android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/xml/filepaths.xml \
    android/src/org/nshchapps/firstyear/activity/QShareActivity.java \
    android/src/org/nshchapps/firstyear/utils/QSharePathResolver.java \
    android/src/org/nshchapps/firstyear/utils/QShareUtils.java
}

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
