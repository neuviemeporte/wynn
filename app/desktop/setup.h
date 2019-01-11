#ifndef SETUP_H
#define SETUP_H

#include <QThread>
#include <QList>

namespace wynn {

namespace db {
class Database;
class Model;
} // namespace db

namespace app {
class MainForm;

class SetupThread : public QThread
{
    Q_OBJECT

private:
    MainForm *parent_;

public:
    SetupThread(MainForm *parent);

    virtual void run();

    static QList<db::Database*> loadDbases(const QString &dirPath, const bool external);

signals:
    void message(const QString &msg);
    void error(const QString &msg);
};

} // namespace app
} // namespace wynn

#endif // SETUP_H
