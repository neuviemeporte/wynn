#include "ext_backend.h"
#include "dict_plugin.h"
#include "dict_table.h"
#include "ixplog_inactive.h"

#include <QPluginLoader>

namespace wynn {
namespace app {

ExtBackend::ExtBackend() : Backend(),
    curPlugin_(nullptr),
    dictModel_(nullptr),
    dictModelRow_(-1)
{
    
}

ExtBackend::~ExtBackend()
{
    // don't need to delete things like the dialogs which are created as children of the main window;
    // they will be destroyed automatically.
	delete dictModel_;

	// Don't need to delete plugins manually here, they will be destroyed automatically when the application terminates.
	for (int i = 0; i < pluginCount(); ++i)	{
		QPluginLoader *loader = pluginLoader(i);
		DictionaryPlugin *plug = qobject_cast<DictionaryPlugin*>(loader->instance());
		Q_ASSERT(plug);
		QLOG("Retrieved loader for plugin " << i << ": '" << plug->name() << "'");
		bool ok = loader->unload();
		QLOG("Unload OK: " << ok << ", destroying loader");
		delete loader;
    }    
}

void ExtBackend::pluginChanged(int plugIdx)
{
    QLOGX("Dictionary language changed to: " << plugIdx);

	if (curPlugin_) curPlugin_->deactivate();
	curPlugin_ = plugin(plugIdx);
	Q_ASSERT(curPlugin_);
	QLOG("Current plugin: " << curPlugin_->name());

	// todo: clear old results from plugins at switch
	dictModel_->setSource(curPlugin_);
    curPlugin_->activate();    
}

void ExtBackend::pluginAbout()
{
    if (!curPlugin_) return;
    emit information(curPlugin_->name(), curPlugin_->description());
}

void ExtBackend::dictDetails()
{
    Q_ASSERT(curPlugin_);
    QLOGX("Showing dictionary search result item details, table row: " << dictModelRow_);
    // TODO: backend should not do UI stuff
	if (dictModelRow_ >= 0) curPlugin_->showResultDetails(dictModelRow_);
}    

void ExtBackend::dictStore()
{
    QLOGX("Dictionary to database button clicked");
	// return if nothing selected in table
    if (dictModelRow_ < 0)
    {
		QLOG("Noting selected in table, aborting");
		return; 
	}

    // extract item/desc data from current plugin
	QStringList data = curPlugin_->getResultForDatabase(dictModelRow_);
    if (data.size() != 2)
    {
		QLOG("Invalid data from plugin");
		return;
	}

    emit dbaseEnterNew(data.first(), data.last());
}

void ExtBackend::dictSearchStart()
{
    QLOGX("Search start");
	dictModel_->beginReset();    
}

void ExtBackend::dictSearchDone()
{
    QLOGX("Search done");
	dictModel_->endReset();
    if ( dictModel_->rowCount(QModelIndex()) ) 
        emit dictResults();
}

void ExtBackend::dictColumnResize(const int col, const int oldSize, const int newSize)
{
	QLOGX("Column " << index << " was resized from " << oldSize << " to " << newSize << ", current plugin: " << plugIdx);
    if (!curPlugin_) 
    {
        QLOG("Current plugin handle is null!");
        return;
    }
    
	curPlugin_->
}

} //namespace app
} //namespace wynn