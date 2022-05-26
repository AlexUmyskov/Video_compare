#-------------------------------------------------
#
# Project created by QtCreator 2022-05-03T18:36:42
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Comparator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

#DEFINES += DHASHERRECT

INCLUDEPATH += /home/sasha/Soft/nl

SOURCES += \
        DHashMatcher.cpp \
        DHasher.cpp \
        DHasherRect.cpp \
        DifferCNN.cpp \
        DifferCorrFrame.cpp \
        DifferCorrHist.cpp \
        DifferDHash.cpp \
        DifferMeanPix.cpp \
        DifferTimeCorrFrame.cpp \
        DifferTimeMeanPix.cpp \
        Utils.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        DHashMatcher.h \
        DHasher.h \
        DHasherRect.h \
        DifferCNN.h \
        DifferCorrFrame.h \
        DifferCorrHist.h \
        DifferDHash.h \
        DifferMeanPix.h \
        DifferTimeCorrFrame.h \
        DifferTimeMeanPix.h \
        Utils.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

CONFIG += link_pkgconfig
PKGCONFIG += /home/sasha/repos/opencv/build/install/lib/pkgconfig/opencv4.pc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
