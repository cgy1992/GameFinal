# Configuration #

The first tutorial will tell how to configure the required development enviroment. This is the first step to write a GameFinal program.

First of all, we need to download the engine and decompress the zip file. Assume that we have now unzipped it under 
the `D:\` directory. Open the engine folder, we'll find the following sub folders:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/00-05.png)

 + **bin:** contains the dll file that is required when running the program.
 + **include:** the cpp header files required during developement.
 + **lib:** the lib file that is needed when compiling and linking the program.
 + **built-in-resources:** some built-in resource files. Usually, we need them in our development.
 + **tools:** contains the mesh generator that is used to convert different models into a ".mesh" file.

We recommand you to use Visual Studio 2012 or 2013 to do the development. Open Visual Studio, we create a new project, just an empty project.

Then click the `Project` menu, select the `Properties` menu item. A property window will pop up. From the left list in the popup window, we select `Configuration Properties->VC++ Directories`, then we need to modify two configurations under this selection: `Include Directories` and `Library Directories`:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/00-02.png)

We add a new directory in the `Include Directories`: "D:\gamefinal_1.0.0_vc11\include", it points to the `include` folder under the engine folder.

We add a new directory in the `Library Directories`: "D:\gamefinal_1.0.0_vc11\lib\debug", it points to the `lib/debug` folder under the engine folder. If you are going to run the program in release mode, you can change it to `lib/release` folder.

At most times, we need to use some built-in resources, so now we'd better to copy the whole `built-in-resources`  folder under our project directory. 

Now, let's add a new cpp file ("main.cpp") in our project, and type in the following code in the cpp file:

	#include <GameFinal.h>
	#pragma comment(lib, "GameFinal.lib")
	
	using namespace gf;
	
	int main()
	{
		IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->createSceneManager();
	
		ITimer* timer = device->getTimer();
		timer->reset();
	
		const f32 color[] = { 0, 0, 0, 1.0f };
		while (device->run()) {
			driver->beginScene(true, true, color);
	
			float dt = timer->tick();
			smgr->update(dt);
			smgr->drawAll();
	
			driver->endScene();
		}
	
		smgr->destroy();
		device->drop();
		return 0;
	}
	 
Then we try to compile and run this program immediately. However, a error window pops up, saying that the program is not able to find `GameFinal.dll` file:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/00-03.png)

So we copy the "GameFinal.dll" from the `bin/debug` folder to your solution's "Debug" directory. This "Debug" folder is created automatically when you're running the program the first time. If you are developing in release mode, you can copy the dll file from `bin/release` folder into the "Release" directory under your solution directory.

Now, try to run the program again. If you can see a console and a window with a black background are showing on the screen, it means you succeed and GameFinal works now!

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/00-04.png)









