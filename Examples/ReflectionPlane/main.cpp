#include <GameFinal.h>
#include <iostream>
#pragma comment(lib, "GameFinal.lib")

using namespace gf;
using namespace std;

int main()
{
	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();

	ITimer* timer = device->getTimer();
	timer->reset();

	// test
	XMFLOAT4 plane(0, 1.0f, 0, 0);
	XMFLOAT3 p(1.0f, 3.0f, -2.0f);
	XMFLOAT3 p2 = math::ComputeMirrorPointAboutPlane(plane, p);
	std::cout << p2.x << "," << p2.y << "," << p2.z << std::endl;

	const f32 color[] = { 0, 1.0f, 0, 1.0f };
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

