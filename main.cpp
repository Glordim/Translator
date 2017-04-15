#include <QApplication>
#include <QTextCodec>

#include "Singleton.h"
#include "LangHelper.h"
#include "MainWindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	LangHelper& langHelper = Singleton<LangHelper>::getInstance();

	if (langHelper.Load("LangInfo.xml") == false)
		return EXIT_FAILURE;

	// UTF-8 Encoding
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);

	MainWindow win;
	win.showMaximized();

	return app.exec();
}
