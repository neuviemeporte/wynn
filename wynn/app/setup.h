#pragma once

#include <QThread>

class MainForm;
class DatabaseModel;
class JmDict;
class Cedict;
class Kanjidic;
class Radkfilex;
class Database;
template <typename T> class QList;

class SetupThread : public QThread
{
	Q_OBJECT

private:
    MainForm *parent_;

public:
	SetupThread(MainForm *parent);

	virtual void run();

    static QList<Database*> loadDbases(const QString &dirPath, const bool external);

signals:
	void message(const QString &msg);
	void error(const QString &msg);
};
