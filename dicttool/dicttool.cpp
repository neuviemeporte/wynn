#include <iostream>
#include <QFile>

#include "dicttool.h"
#include "ixpdict.h"
#include "util.h"
#include "ixplog_active.h"

QTSLogger 
	dicttool_log("dicttool_log.txt"),
	*dicttool_logstream = &dicttool_log;

#define QLOGSTREAM dicttool_logstream

QTextStream qin(stdin), qout(stdout);

const QString 
	Task::KANJIDIC_XML     = "dictionaries/kanjidic2.xml",
	Task::KANJIDIC_DATA    = "dictionaries/kanjidic.dat",
	Task::KANJIDIC_UH_DATA = "dictionaries/kanjidic-uh.dat",
	Task::RADKFILEX_TEXT   = "dictionaries/radkfilex.utf8",
	Task::RADKFILEX_DATA   = "dictionaries/radkfilex.dat",
	Task::JMDICT_XML       = "dictionaries/JMDict_e.xml",
	Task::JMDICT_DATA      = "dictionaries/JMDict_en.dat",
	Task::CEDICT_TEXT      = "dictionaries/cedict_ts.u8",
	Task::CEDICT_DATA      = "dictionaries/cedict.dat",
	Task::BEO_TEXT         = "dictionaries/de-en-2013.txt",
	Task::BEO_DATA         = "dictionaries/de_en.dat";

void Task::run()
{
	IXPDICT_LOGREDIR(dicttool_logstream);
    bool quit = false;

    while (!quit) 
    {
        qout << "DICTTOOL - repackage various dictionaries off the 'net" << endl
             << "into binary format for use with Wynn." << endl
             << "All dictionaries are (C) by their respective copyright owners" << endl
             << "==========================================================================" << endl
             << "1 - Chinese character dictionary (KANJIDIC and Unihan database)" << endl
             << "2 - Chinese character radical lookup database (RADKFILEX)" << endl
             << "3 - Japanese vocabulary dictionary (JMDict)" << endl
             << "4 - Chinese vocabulary dictionary (CEDICT)" << endl
             << "5 - German dictionary (Beolingus)" << endl
             << "6 - Prepare all (perform steps 1-5) and exit" << endl // todo: out
             << "7 - Exit" << endl;        
        
        const QString selectionStr = qin.readLine();
        bool ok = false, all = false;
        const int selection = selectionStr.toInt(&ok);
        
        if (!ok) 
        {
            qout << "Invalid input: '" << selectionStr << "'" << endl;
            break;
        }
        
        switch (selection)
        {
        case 6:
            all = true; // fallthrough
        case 1:
            processUnihanKanjidic();
            if (!all) break; // fallthrough
        case 2:
            processRadkfile();
            if (!all) break; // fallthrough
        case 3:
            processJMDict();
            if (!all) break; // fallthrough
        case 4:
            processCedict();
            if (!all) break; // fallthrough
        case 5:
            processBeolingus();
            if (!all) break; // fallthrough
        case 7:
            qout << "Exiting..." << endl;
            quit = true;
            break;
        default:
            qout << "Invalid selection: " << selection << endl;
            break;
        }

    }

	emit finished();
}

// Parsing Unihan + Kanjidict, complementing Kanjidict with Unihan data 
void Task::processUnihanKanjidic()
{
	Unihan u;
	std::cout << "Parsing Unihan database... ";
	u.parse();
	qout << "Read " << u.hashEntryCount() << " entries" << endl;

	Kanjidic kd, kd2;
	qout << "Parsing XML KANJIDIC (" << KANJIDIC_XML << ") ..." << flush;
	kd.parseXML(KANJIDIC_XML);
	qout << endl;

	qout << "Read " << kd.entryCount() << " items, saving to binary file (" << KANJIDIC_DATA << ") ..." << flush;
	kd.saveData(KANJIDIC_DATA);
	qout << endl;

	if (!u.error())
	{
		qout << "Reading back binary KANJIDIC... ";
		kd2.loadData(KANJIDIC_DATA);
		if (kd2.error())
		{
			qout << "Corrupt data reading the binary kanjidic file, terminating" << endl;
		}
		else
		{
			qout << "Read " << kd2.entryCount() << " items." << endl 
				<< "Updating KANJIDIC with Unihan info... " << flush;
			u.updateKanjidic(kd2);
			qout <<  "Kanjidic now has " << kd2.entryCount() << " entries." << endl 
				<< "Saving updated dictionary (" << KANJIDIC_UH_DATA << ")" << endl;
			kd2.saveData(KANJIDIC_UH_DATA);
		}
	}
	else
	{
		qout << "There were errors parsing the Unihan database, terminating" << endl;
	}

	//for (int i = 0; i < kd.entryCount(); ++i)
	//{
	//	const HanCharacter &kanji = kd.entry(i);
	//	const UnihanCharInfo c = u.entry(kanji.character());

	//	if (c.pinyin_.isEmpty()) LOG("Character '" << kanji.character() << "' has no pinyin in Unihan!");
	//	else LOG("Character '" << kanji.character() << "' has pinyin '" << kanji.pinyinStr() << "' in Kanjidic and '"
	//		<< Util::strListMerge(c.pinyin_, "/") << "' in Unihan");
	//}
}

void Task::processRadkfile()
{
	Radkfilex r;
	qout << "Parsing RADKFILEX (" << RADKFILEX_TEXT << ") ..." << flush;
	r.parse(RADKFILEX_TEXT);
	qout << endl;

	qout << "Writing in binary format (" << RADKFILEX_DATA << ") ..." << flush;
	r.saveData(RADKFILEX_DATA);
	qout << endl;
}

void Task::processJMDict()
{
	JmDict e;
	qout << "Parsing JmDict XML (" << JMDICT_XML << ") ..." << flush;
	e.parseXML(JMDICT_XML);
	qout << endl;

	qout << "Writing in binary format (" << JMDICT_DATA << ") ... " << flush;
	e.saveData(JMDICT_DATA);
	qout << endl;
}

void Task::processCedict()
{
	Cedict cedict, c2;
	qout << "Parsing CEDICT (" << CEDICT_TEXT << ") ... " << flush;
	cedict.parse(CEDICT_TEXT);

	qout << endl << "Read " << cedict.entryCount() << " entries (error: " << cedict.error() << ")." << endl
		<< "Saving to binary file (" << CEDICT_DATA << ") ..." << flush;
	bool ok = cedict.saveData(CEDICT_DATA);
	qout << endl << "done (ok: " << ok << ")." << endl << "Reloading from binary file... " << flush;
	c2.loadData(CEDICT_DATA);
	qout << "Reloaded with " << c2.entryCount() << " entries (error: " << c2.error() << ")." << endl;
}

void Task::processBeolingus()
{
	BeoDict beo, beo2;
	qout << "Parsing Beolingus (" << BEO_TEXT << ") ..." << flush;
	beo.parse(BEO_TEXT);
	qout << endl << "Beolingus dictionary parsed with " << beo.entryCount() << " entries, error: " << beo.error() << endl
		<< "Saving as binary file (" << BEO_DATA << ") ..." << flush;
	beo.saveData(BEO_DATA);
	qout << endl;

	beo2.loadData(BEO_DATA);
	qout << "Reloaded with " << beo2.entryCount() << " entries, error: " << beo2.error() << endl;
}

void Task::testBeolingus()
{
	BeoDict beo;
	qout << "Loading Beolingus (" << BEO_DATA << ") ..." << flush;
	beo.loadData(BEO_DATA);
	qout << endl << "Beolingus loaded: " << beo.entryCount() << " entries, error: " << beo.error() << endl;

	if (beo.error()) return;

	QString input;
	do
	{
		qout << "================================================" << endl << "Input query: " << flush;
		input = qin.readLine(75);
		if (input.isEmpty()) break;

		const QStringList query(input);
		const QList<BeoResult> results = beo.search(BEO_DE, query, EXACT);
	
		if (results.size()) for (int i = 0; i < results.size(); ++i)
		{
			const BeoResult &result = results.at(i);
			qout << i << " (" << result 
				<< "): DE = '" << beo.resultText(BEO_DE, result) 
				<< "', EN = '" << beo.resultText(BEO_EN, result.allPhrases()) 
				<< "'" << endl;
		}
		else
		{
			qout << "No results" << endl;
		}
	} while (true);
	
}
