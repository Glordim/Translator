#include <QtXml>
#include <QFileInfo>
#include <QTextCodec>
#include <QMessageBox>
#include <QApplication>
#include <QCommandLineParser>

#include "Singleton.h"
#include "MainWindow.h"
#include "LangHelper.h"

struct Preference
{
	QString lastProjectPath;
};

Preference GetPreference()
{
	Preference preference;

	QString configLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

	if (configLocation.isEmpty() == true)
		return preference;

	QDir configDir(configLocation);

	if (configDir.exists() == false)
	{
		return preference;
	}

	QFile configFile(configDir.absolutePath() + "/" + "preference.cfg");

	if (configFile.open(QIODevice::ReadOnly) == false)
	{
		QMessageBox::critical(NULL, "Erreur", "Impossible d'ouvrir en lecture le projet");
		return preference;
	}

	QDomDocument* dom = new QDomDocument(configFile.fileName());

	if (dom->setContent(&configFile) == false)
	{
		configFile.close();
		return preference;
	}

	configFile.close();

	QDomElement docElem = dom->documentElement();
	QDomNode node = docElem.firstChild();

	while (node.isNull() == false)
	{
		QDomElement element = node.toElement();

		if (element.tagName() == "Line")
		{
			if (element.hasAttribute("LastProject") == true)
				preference.lastProjectPath = element.attribute("LastProject");
		}

		node = node.nextSibling();
	}

	return preference;
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("Translator");
	QApplication::setApplicationVersion("1.1");

	// UTF-8 Encoding
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);

	QCommandLineParser parser;
	parser.setApplicationDescription("Usage: Translator [project-path]");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	LangHelper& langHelper = Singleton<LangHelper>::getInstance();

	if (langHelper.Load(QApplication::applicationDirPath() + "/LangInfo.xml") == false)
		return EXIT_FAILURE;

	MainWindow win;
	win.showMaximized();

	const QStringList args = parser.positionalArguments();

	if (args.count() > 0)
	{
		win.LoadProject(args.at(0));
	}
	else
	{
		Preference preference = GetPreference();

		if (preference.lastProjectPath.isEmpty() == false)
			win.LoadProject(preference.lastProjectPath);
	}

	return app.exec();
}
