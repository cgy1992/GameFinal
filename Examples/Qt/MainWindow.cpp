#include "mainwindow.h"

MainWindow* g_mainWindow = nullptr;

static bool game_update(f32 dt) {
	return g_mainWindow->game_update(dt);
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent)
{
	
}

bool MainWindow::game_update(f32 dt)
{
	const f32 color[] = { 0, 0, 0, 1.0f };

	//    smgr->update(dt);
	mDriver->beginScene(true, true, color);
	//    smgr->drawAll();
	mDriver->endScene();
	return true;
}


MainWindow::~MainWindow()
{
	mApp->drop();
}

void MainWindow::init()
{
	g_mainWindow = this;

	SAppSettings settings;
	settings.AppType = EAP_QT5;
	settings.Window.FrameWindowPtr = this;
	mApp = gf::createApp(settings);

	mDriver = mApp->getVideoDriver();
	mSmgr = mApp->createSceneManager();

	mApp->setUpdateCallback(::game_update);
	mApp->run();
}
