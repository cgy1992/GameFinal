#include<Windows.h>
#include<GameFinal.h>
#include "mainwindow.h"
#include <QApplication>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "qtmaind.lib")
#pragma comment(lib, "Qt5Widgetsd.lib")
#pragma comment(lib, "Qt5Guid.lib")
#pragma comment(lib, "Qt5Cored.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	w.init();

	return a.exec();
}