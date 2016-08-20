#include <GameFinal.h>
#include <BlockQueue.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

static long num_steps = 1000000; double step;
static int num_tasks = 1000;

static void job_proc(int id, double* sum) {
	double s = 0.0;
	double x;
	for (int i = id; i < num_steps; i = i + num_tasks) {
		x = (i + 0.5) * step;
		s += 4.0 / (1.0 + x * x);
	}
	*sum = s;
}


int main()
{
	SAppSettings settings;
	settings.Thread.ThreadPoolLimit = 4;
	IApplication* app = createApp(settings);

	IVideoDriver* driver = app->getVideoDriver();
	ISceneManager* smgr = app->createSceneManager();

	step = 1.0 / (double)num_steps;
	std::vector<double> sums(num_tasks);

	IThreadPool* pool = app->getThreadPool();
	SJobFunctionSet jobSet;

	for (size_t i = 0; i < num_tasks; i++) {
		jobSet.push_back(std::bind(job_proc, i, &sums[i]));
	}

	pool->addJobs(jobSet, [&sums]() {
		double sum = 0;
		for (size_t i = 0; i < num_tasks; i++) {
			sum += sums[i] * step;
		}
		printf("pi = %f \n", sum);
	});

	app->setUpdateCallback([app, driver, smgr](float dt)->bool{
		const f32 color[] = { 0, 0, 0, 1.0f };
		driver->beginScene(true, true, color);
		smgr->update(dt);
		smgr->drawAll();
		driver->endScene();
		return true;
	});

	
	app->run();
	smgr->destroy();
	app->drop();
	return 0;
}

