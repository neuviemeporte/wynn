#include "setup.h"
#include "window.h"
#include "dict_plugin.h"
#include "global.h"
#include "database.h"

#include <ixplog_active.h>

#include <QDir>
#include <QMessageBox>
#include <QString>

SetupThread::SetupThread(MainForm *parent) : QThread(parent), 
	parent_(parent)
{
	connect(this, SIGNAL(finished()),               parent, SLOT(slot_setupDone()),                     Qt::QueuedConnection);
	connect(this, SIGNAL(message(const QString &)), parent, SLOT(slot_updateSetupMsg(const QString &)), Qt::QueuedConnection);
}

void SetupThread::run()
{
	QLOGX("Running setup thread");
	QLOGINC;

	QLOG("Setting up dictionary plugins");

	for (int i = 0; i < parent_->pluginCount(); ++i)
	{
		DictionaryPlugin *plugin = parent_->plugin(i);
		const QString plugName = plugin->name();
		QLOG("Plugin " << i << ": '" << plugName << "'");
		emit message("Setting up plugin: " + plugName);

		if (plugin->ok())
		{
			plugin->setup();
		}
		else
		{
			QLOG("Plugin in invalid state, skipping");
		}
	}

	emit message("Loading user databases...");
	QLOG("Parsing user database files");
    // create databases from the home directory and the external directory
    // and move them to the main thread
    QList<Database*> dbs;
    dbs.append(loadDbases(parent_->APPDIR, false));
    dbs.append(loadDbases(parent_->extDir_, true));
    QLOG("Got " << dbs.size() << "in total");
    QThread *appThread = QApplication::instance()->thread();
    for (int i = 0; i < dbs.size(); ++i)
    {
        Database *db = dbs.at(i);
        db->moveToThread(appThread);
        parent_->addDatabase(db);
    }

	QLOGDEC;
}

QList<Database*> SetupThread::loadDbases(const QString &dirPath, const bool external)
{
    QLOGX("Scanning directory '" << dirPath << "' for user databases");
    QLOGINC;

    QList<Database*> ret;
    QDir dir(dirPath);
    if (dirPath.isEmpty() || !dir.exists())
    {
        QLOG("Directory unreachable, exiting");
        if (!external)
        {
            QLOG("Creating missing directory");
            dir.mkpath(dirPath);
        }
        QLOGDEC;
        return ret;
    }

    // find all xml files within directory and store their paths
    QStringList files = dir.entryList(QStringList("*" + Database::XML_EXT));
    QLOG("Found " << Database::XML_EXT << " files: " << files.size());

    Database *dbase;
    // iterate over paths and create databases from them, adding handles to a list
    for (int i = 0; i < files.size(); ++i)
    {
        const QString &file = files.at(i);
        QLOG("Database file: '" << file << "'");
        QLOGINC;
        dbase = new Database(NULL);
        Q_ASSERT(dbase != NULL);

        DbError error = dbase->loadXML(dirPath + "/" + file);
        if (error == DbError::ERROR_OK)
        {
            QLOG("Database in order, appending to list (entries: " << dbase->entryCount() << ")");
            dbase->setExternal(external);
            ret.append(dbase);
        }
        else
        {
            QLOG("Error reading this database!");
            // todo: ujednolicic to i reloaduserdatabases, gdzies razem trzymac
            // nie mozna msgboxa bo to wolane z setupthreada a w innym threadzie nie mozna tworzyc widgetow
            // QMessageBox::warning(this, tr("Error!"), tr("Couldn't read user database\n") + error.msg);
            delete dbase;
        }
        QLOGDEC;
    }

    QLOG("Directory processed, databases loaded: " << ret.size());
    QLOGDEC;
    return ret;
}

