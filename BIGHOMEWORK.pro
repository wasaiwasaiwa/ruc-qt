QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

QMAKE_CXXFLAGS += -Wa,-mbig-obj

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../ThirdLibrary

SOURCES += \
    main.cpp \
    widget.cpp \
    qcustomplot.cpp \
    t1.cpp \
    t2.cpp \
    t3.cpp \
    t4.cpp \
    t5.cpp

HEADERS += \
    widget.h \
    qcustomplot.h \
    t1.h \
    t2.h \
    t3.h \
    t4.h \
    t5.h


FORMS += \
    widget.ui

DESTDIR = ../../bin
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
