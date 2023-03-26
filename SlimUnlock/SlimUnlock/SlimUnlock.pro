QT       += core gui
QT       += network
QT       += core5compat
QT       += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    FileTools.hpp \
    NetworkTools.hpp \
    ProcessTools.hpp \
    mainwindow.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    apps/HuananzhiParser/main.exe \
    apps/ami/AMIBCP5.exe \
    apps/mmtool/mmtool_a5.exe \
    apps/unrar/slimrg.exe \
    apps/unrar/unrar.exe

RESOURCES += \
    Apps.qrc

RC_ICONS = assets/app.ico

QMAKE_LFLAGS_RELEASE += -static -static-libgcc -static-libstdc++

INCLUDEPATH += $$PWD/libs/openssl-static/include
LIBS += -L$$PWD/libs/openssl-static/lib64 -lssl -lcrypto
