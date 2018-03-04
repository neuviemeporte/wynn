#ifndef WYNN_TEST_H_
#define WYNN_TEST_H_

#include <QtCore>

#include "ixp_test.h"

class TestTask : public QObject
{
	Q_OBJECT

public:
	TestTask(QObject *parent = 0) : QObject(parent) {}

public slots:
	void run();

signals:
	void finished();
};


class DatabaseTest : public ixp::Test
{
public:
    DatabaseTest() : ixp::Test("Database module") {}

protected:
    virtual void execute();
};

#endif // WYNN_TEST_H_
