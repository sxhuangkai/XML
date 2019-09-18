#include "xml.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.readBookInfoFile();
	w.readStaffInfoFile();
	w.show();
	return a.exec();
}
