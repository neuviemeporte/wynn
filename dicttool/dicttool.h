#ifndef DICTTOOL_H_
#define DICTTOOL_H_

#include <QtCore>

class Task : public QObject
{
	Q_OBJECT
private:
	static const QString 
		KANJIDIC_XML, KANJIDIC_DATA, KANJIDIC_UH_DATA,
		RADKFILEX_TEXT, RADKFILEX_DATA,
		JMDICT_XML, JMDICT_DATA,
		CEDICT_TEXT, CEDICT_DATA,		
		BEO_TEXT, BEO_DATA;

public:
	Task(QObject *parent = 0) : QObject(parent) {}

public slots:
	void run();

signals:
	void finished();

protected:
	void processUnihanKanjidic();
	void processRadkfile();
	void processJMDict();
	void processCedict();

	void processBeolingus();
	void testBeolingus();
};

#endif // DICTTOOL_H_
