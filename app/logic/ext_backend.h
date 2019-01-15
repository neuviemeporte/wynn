#ifndef EXT_BACKEND_H
#define EXT_BACKEND_H

#include "backend.h"
#include <QList>

class DictionaryPlugin;
class QPluginLoader;
class DictionaryModel;

namespace wynn {
namespace app {

// Extended backend, with plugin-based dictionary capabilities
class ExtBackend : public Backend {
    // TODO: change to vector everywhere
    QList<DictionaryPlugin*> plugins_;
    QList<QPluginLoader*> pluginLoaders_;
    DictionaryPlugin *curPlugin_;
    DictionaryModel *dictModel_;

    // properties bound to UI controls    
    Q_PROPERTY(int dictModelRow MEMBER dictModelRow_)
    int dictModelRow_;
    
public:
    ExtBackend();
    ~ExtBackend();
    
public slots:
    void pluginChanged(int plugIdx);
    void pluginAbout();
    void dictDetails();
    void dictStore();
    
signals:
    
    
protected:
	int pluginCount() const { return plugins_.size(); }
	DictionaryPlugin* plugin(const int index) { return plugins_.at(index); }
	QPluginLoader* pluginLoader(const int index) { return pluginLoaders_.at(index); }
    
};

} //namespace app
} //namespace wynn

#endif // EXT_BACKEND_H
