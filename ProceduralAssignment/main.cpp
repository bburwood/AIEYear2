
#include <cstdio>
#include "ProceduralGeneration.h"

int main()
{
	//	IntroToOpenGL	app;
	//	CameraAndProjections	app;
	//	RenderingGeometry	app;
	//	Texturing app;
	//	Lighting app;
	//	AdvancedTexturing app;
	//	Quaternions	app;
	//	BLANK	app;
	//	Animation	app;
	//	Particles	app;
	//	GPUParticles	app;
	//	SceneManagement	app;
	//	RenderTargets	app;
	//	PostProcess	app;
	//	Shadows	app;
	//	Deferred	app;
	ProceduralGeneration	app;
	//	AdvancedNavigation	app;
	//	PhysicallyBasedRendering	app;

	if (app.startup() == false)
	{
		return -1;
	}

	while (app.update())
	{
		app.draw();
	}

	app.shutdown();
	return 0;
}
