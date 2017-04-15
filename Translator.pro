#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T17:40:58
#
#-------------------------------------------------

QT       += core gui xml webengine webenginewidgets multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Translator
TEMPLATE = app

SOURCES += main.cpp\
    MainWindow.cpp \
    DialogCreateProject.cpp \
    DialogProxy.cpp \
    ProjectHelper.cpp \
    LangHelper.cpp

HEADERS  += MainWindow.h \
    DialogCreateProject.h \
    DialogProxy.h \
    ProjectHelper.h \
    LangHelper.h \
    Singleton.h

FORMS    += MainWindow.ui \
    DialogCreateProject.ui \
    DialogProxy.ui

RESOURCES += \
    resources.qrc
