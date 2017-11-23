#-------------------------------------------------
#
# Project created by QtCreator 2013-10-19T14:10:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ieffect
TEMPLATE = app

include(effects/effects.pri)

SOURCES += main.cpp\
    imagequeue.cpp \
    pipelinemanager.cpp \
    pipelinestage.cpp \
    effectstage.cpp \
    simpletracer.cpp \
    imageloaderstage.cpp \
    imagesaverstage.cpp

HEADERS  += \
    imagequeue.h \
    pipelinemanager.h \
    pipelinestage.h \
    effectstage.h \
    simpletracer.h \
    imagesaverstage.h \
    imageloaderstage.h \

FORMS    +=

OTHER_FILES += \
    effects/effects.pri
