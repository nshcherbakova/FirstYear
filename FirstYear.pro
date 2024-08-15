QT       += core gui
# QT = core gui widgets openglwidgets opengl network svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PROJECT_PATH = $$PWD
PROJECT_THIRDPARTY = /Users/nshcherbakova/Documents/dev/Thirdparty

INCLUDEPATH +=$$PROJECT_PATH/Core/Gears/StdAfx/ \
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
    UI/FrameWidgets/DefaultFrameWidget.cpp \
    UI/FrameWidgets/ImageButton.cpp \
    UI/FrameWidgets/PhotoWidget.cpp \
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
    UI/FrameWidgets/DefaultFrameWidget.h \
    UI/FrameWidgets/ImageButton.h \
    UI/FrameWidgets/PhotoWidget.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
