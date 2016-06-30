#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <GameFinal.h>

using namespace gf;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	//Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);

	bool game_update(f32 dt);

	void init();

	~MainWindow();

private:
	IApplication*       mApp;
	IVideoDriver*       mDriver;
	ISceneManager*      mSmgr;
};

#endif // MAINWINDOW_H
