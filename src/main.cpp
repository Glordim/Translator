#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QXmlStreamReader>
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

	QFile file(configDir.absolutePath() + "/" + "preference.cfg");

	if (file.open(QIODevice::ReadOnly) == false)
	{
		QMessageBox::critical(NULL, "Erreur", "Impossible d'ouvrir en lecture le projet");
		return preference;
	}

	QXmlStreamReader xml(&file);

	while (xml.atEnd() == false)
	{
		xml.readNext();

		if (xml.isStartElement() == true)
		{
			if (xml.name() == "Line")
			{
				QXmlStreamAttributes attributesVec = xml.attributes();

				for (int i = 0; i < attributesVec.count(); ++i)
				{
					QXmlStreamAttribute attribute = attributesVec[i];
					QString attributeName = attribute.name().toString();

					if (attributeName == "LastProject")
					{
						preference.lastProjectPath = attribute.value().toString();
					}
				}
			}
		}
	}

	file.close();

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
