
#include <QtCore/QCoreApplication>
#include "wynn_test.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	TestTask *task = new TestTask(&a);
	QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));
	QTimer::singleShot(0, task, SLOT(run()));

	return a.exec();
}
