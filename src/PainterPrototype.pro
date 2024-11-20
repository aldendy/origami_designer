#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T11:11:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PainterApplication
TEMPLATE = app


SOURCES += \
    main.cpp \
    Window.cpp \
    RenderArea.cpp \
    crease.cpp \
    creasepattern.cpp \
    vertex.cpp \
    rowreduce.cpp

HEADERS  += \
    renderarea.h \
    window.h \
    crease.h \
    creasepattern.h \
    vertex.h \
    rowreduce.h

RESOURCES += \
    basicdrawing.qrc

FORMS +=
