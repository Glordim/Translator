#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T17:40:58
#
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Translator
TEMPLATE = app

### WebEngine ###

#DEFINES += USE_WEBENGINE

contains(DEFINES, USE_WEBENGINE)
{
	QT += webengine webenginewidgets multimedia network

	SOURCES += src/DialogProxy.cpp
	HEADERS += src/DialogProxy.h
	FORMS += src/DialogProxy.ui
}

###

SOURCES += src/main.cpp \
	   src/MainWindow.cpp \
	   src/DialogCreateProject.cpp \
	   src/ProjectHelper.cpp \
	   src/LangHelper.cpp

INCLUDEPATH += $$PWD/src

HEADERS += src/MainWindow.h \
	   src/DialogCreateProject.h \
	   src/ProjectHelper.h \
	   src/LangHelper.h \
	   src/Singleton.h

UI_DIR = $$PWD/src

FORMS += src/MainWindow.ui \
	 src/DialogCreateProject.ui

RESOURCES += resources.qrc

OTHER_FILES += $$PWD/LangInfo.xml

### Copy LangInfo.xml ###

defineTest(copyToDestdir) {

	files = $$ARGS

	message("Start Copy... $$files")

	for(FILE, files) {

		DDIR = $$OUT_PWD

		# Replace slashes in paths with backslashes for Windows
		win32:FILE ~= s,/,\\,g
		win32:DDIR ~= s,/,\\,g

		message($$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR))

		QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	message("End Copy.")

	export(QMAKE_POST_LINK)
}

copyToDestdir($$OTHER_FILES)
