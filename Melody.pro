QT       += core gui network multimedia sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    listwidgetlocal.cpp \
    main.cpp \
    musiceditdialog.cpp \
    myplaylist.cpp \
    showitem.cpp \
    showplaylistitem.cpp \
    showplaylistmusicitem.cpp \
    showtableitem.cpp \
    usemysql.cpp \
    usenetwork.cpp \
    usesqlite.cpp \
    widget.cpp

HEADERS += \
    listwidgetlocal.h \
    music.h \
    musiceditdialog.h \
    myplaylist.h \
    showitem.h \
    showplaylistitem.h \
    showplaylistmusicitem.h \
    showtableitem.h \
    usemysql.h \
    usenetwork.h \
    usesqlite.h \
    widget.h

FORMS += \
    musiceditdialog.ui \
    showitem.ui \
    showplaylistitem.ui \
    showplaylistmusicitem.ui \
    showtableitem.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
