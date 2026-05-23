QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = EasyNote

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(QxtGlobalShortcut/qxtglobalshortcut.pri)

win32 {
    QMAKE_CXXFLAGS += /MP
    LIBS += -lshell32
}

SOURCES += \
    aboutdialog.cpp \
    helpfunc.cpp \
    finddialog.cpp \
    helpdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    notewidget.cpp \
    renamedialog.cpp \
    setdialog.cpp \
    singleapplication.cpp \
    codeeditor.cpp \
    filetreewidget.cpp

HEADERS += \
    aboutdialog.h \
    helpfunc.h \
    finddialog.h \
    helpdialog.h \
    mainwindow.h \
    notewidget.h \
    renamedialog.h \
    setdialog.h \
    singleapplication.h \
    codeeditor.h \
    filetreewidget.h

FORMS += \
    aboutdialog.ui \
    finddialog.ui \
    helpdialog.ui \
    mainwindow.ui \
    notewidget.ui \
    renamedialog.ui \
    setdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    RC_FILE = EasyNote.rc
}

RESOURCES += \
    resource.qrc

contains(CONFIG, sponsor) {
    DEFINES += ENABLE_SPONSOR
    RESOURCES += sponsor.qrc
}
