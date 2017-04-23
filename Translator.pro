#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T17:40:58
#
#-------------------------------------------------

QT       += core gui xml webengine webenginewidgets multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Translator
TEMPLATE = app

defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
	DDIR = $$DESTDIR

	# Replace slashes in paths with backslashes for Windows
	win32:FILE ~= s,/,\\,g
	win32:DDIR ~= s,/,\\,g

	QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestdir($$PWD/LangInfo.xml)

SOURCES += src/main.cpp\
    src/MainWindow.cpp \
    src/DialogCreateProject.cpp \
    src/DialogProxy.cpp \
    src/ProjectHelper.cpp \
    src/LangHelper.cpp

INCLUDEPATH += $$PWD/src

HEADERS  += src/MainWindow.h \
    src/DialogCreateProject.h \
    src/DialogProxy.h \
    src/ProjectHelper.h \
    src/LangHelper.h \
    src/Singleton.h

UI_DIR = $$PWD/src

FORMS    += src/MainWindow.ui \
    src/DialogCreateProject.ui \
    src/DialogProxy.ui

RESOURCES += resources.qrc
