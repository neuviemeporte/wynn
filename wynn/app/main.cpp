#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("IXP");
	a.setApplicationName("Wynn");
	MainForm w;
	w.show();
	return a.exec();
}
