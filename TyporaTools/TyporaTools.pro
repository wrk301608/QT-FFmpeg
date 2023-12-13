QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#引入托盘模块
include($$PWD/TrayIcon/TrayIcon.pri)
INCLUDEPATH += ./TrayIcon


#引入日志模块
include($$PWD/MyLog/MyLog.pri)
INCLUDEPATH += ./MyLog


#引入GIF制作模块
include($$PWD/GIFMake/GIFMake.pri)
INCLUDEPATH += ./GIFMake

#引入代码统计模块
include($$PWD/CodeCount/CodeCount.pri)
INCLUDEPATH += ./CodeCount

#引入截屏模块
include($$PWD/ScreenCap/ScreenCap.pri)
INCLUDEPATH += ./ScreenCap

#引入录制模块
include($$PWD/VideoCap/VideoCap.pri)
INCLUDEPATH += ./VideoCap


SOURCES += \
    main.cpp \
    mainwidget.cpp

HEADERS += \
    mainwidget.h

FORMS += \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
