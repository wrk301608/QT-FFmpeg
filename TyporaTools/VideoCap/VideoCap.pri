

LIBS += -L$$PWD/../../FFmpegBag/ffmpeg/lib/ -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice
INCLUDEPATH += $$PWD/../../FFmpegBag/ffmpeg/include
DEPENDPATH += $$PWD/../../FFmpegBag/ffmpeg/include



HEADERS += \
    $$PWD/readthread.h \
    $$PWD/videocodec.h \
    $$PWD/videodecode.h

SOURCES += \
    $$PWD/readthread.cpp \
    $$PWD/videocodec.cpp \
    $$PWD/videodecode.cpp
