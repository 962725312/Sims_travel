QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = Run
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Core_sims.cpp \
    Log.cpp \
    dialog_insert.cpp \
    init_data.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Core_sims.h \
    Log.h \
    dialog_insert.h \
    init_data.h \
    mainwindow.h

FORMS += \
    dialog_insert.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    C:/Users/1/Desktop/init_lines_map.txt \
    data/init_lines_map.txt \
    data/log_file.txt

RESOURCES += \
    icon.qrc \
    image.qrc \
    initdata.qrc
