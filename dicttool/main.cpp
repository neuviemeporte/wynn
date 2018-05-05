#include <QtCore/QCoreApplication>
#include <memory>

#include "dicttool.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	std::unique_ptr<Task> task = std::make_unique<Task>(&a);
	QObject::connect(task.get(), SIGNAL(finished()), &a, SLOT(quit()));
	QTimer::singleShot(0, task.get(), SLOT(run()));
	return a.exec();
}
