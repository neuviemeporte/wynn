#ifndef DICT_PLUGIN_H
#define DICT_PLUGIN_H

#include <QtPlugin>
#include <QFont>
#include <QWidget>

// The generic interface for plugins adding functionality of new languages to the main application. 
// You need to inherit this and provide implementations for all the pure virtual functions.
//
// When building a plugin, the contents of this file need to be available to moc when processing the plugin's
// header file to avoid the "Undefined interface" error. It may be necessary to manually add the directory
// containing this file with the -I parameter to the moc commandline for the particular plugin's header
// (In Visual Studio look under the header's Properties -> Custom build tool).
class DictionaryPlugin
{
protected:
	int index_;
	bool ok_;
	QWidget *dictWidget_, *settWidget_;

public:
	DictionaryPlugin() : index_(-1), ok_(true), dictWidget_(NULL), settWidget_(NULL) {}

	virtual ~DictionaryPlugin() 
	{
		delete dictWidget_;
		delete settWidget_;
	}

	// Status of plugin. This is checked after load by the main application. Return false if plugin failed to initialize (e.g. unable to load dictionary).
	virtual bool ok() const { return ok_; }
	// The plugin's index. This is the index under which the application stores this plugin.
	virtual int index() const { return index_; }    
	// The application calls this for all plugins which are still ok() after postSetup() has been called. 
	virtual void setIndex(const int pluginIndex) { index_ = pluginIndex; }
	// The UI of the dictionary panel for this language. Ownership is not passed, the plugin should dispose of it when destroyed.
	virtual QWidget* dictionaryUI() const { return dictWidget_; }
	// The UI of the settings panel for this language. Ownership is not passed, the plugin should dispose of it when destroyed.
	virtual QWidget* settingsUI() const { return settWidget_; }
	// If the language needs a special font for displaying text, it needs to be returned here. Otherwise just return QFont().
	virtual QFont font() const { return QFont(); }

	// A string identifying the name of the plugin, e.g. "Jill's Fancy Tamil Language plugin v1.2".
	virtual QString name() const = 0;
	// A string identifying the name of the language, e.g. "Tamil".
	virtual QString language() const = 0;
	// An optional string containing additional details about the plugin. This is displayed in the plugin "About" dialog
	// and can be more verbose, including rich text (HTML).
	virtual QString description() const { return QString(); }

	// The following functions are queried by the application's dictionary search results table model. Reimplement them to provide search results
	// for display in the table.
	virtual int resultItemCount() const = 0;
	virtual int resultColumnCount() const = 0;
	virtual QString resultData(const int itemIndex, const int columnIndex) const = 0;
	virtual QString resultHeaderData(const int column) const = 0;

	// Called by the application directly when details about a dictionary search result item were requested. 
	// The plugin can show the details, e.g. in a dialog window
	virtual void showResultDetails(const int resultIndex) = 0;
	// Called by the application directly when a result item was requested for add to database. 
	// The first two strings in the returned list are used to fill the "Entry" and "Description" edits in the add-to-database dialog.
	virtual QStringList getResultForDatabase(const int resultIndex) = 0;

	// This is called by the application after the plugin is loaded, and executes on the main thread. Useful for simple pre-setup initialization tasks
	// that should normally be taken care of in the constructor, but since that needs to be called with no arguments for plugins, the scope of what it
	// can do is rather limited. Here we are passed a handle to the main application so we can connect signals between it and the plugin.
	// Also passed is a pointer to the application's logging stream, so the plugin may substitute it for its own and that its log outuput will
	// appear in the application's log.
	virtual void preSetup(QWidget *parent, class QTSLogger *debugLog) 
	{
		Q_UNUSED(parent);
		Q_UNUSED(debugLog);
	}

	// This is called by the application after the plugin is loaded, and preSetup() completed, if ok() returned true. 
	// Since this executes in a different thread, it is not possible to create widgets here. It is instead useful for non-UI-related
	// setup tasks that take a noticeable amount of time, like dictionary load.
	virtual void setup() {}

	// This will be called by the application on the main thread after the plugin was loaded, setup() completed and ok() returned true. 
	// If needed, reimplement this function to take care of additional setup tasks that rely on the results of setup().
	// This should not take a long time, because it will block the application's main thread.
	virtual void postSetup() {}

	// When the currently selected plugin changes in the Dictionary panel, the application calls this for the plugin that has
	// been switched to. Reimplement if you need to react to this event.
	virtual void activate() {}

	// When the currently selected plugin changes in the Dictionary panel, the application calls this for the plugin that has
	// been switched away from. Reimplement if you need to react to this event.
	virtual void deactivate() {}
};

Q_DECLARE_INTERFACE(DictionaryPlugin, "wynn.DictionaryPlugin")

#endif // DICT_PLUGIN_H
