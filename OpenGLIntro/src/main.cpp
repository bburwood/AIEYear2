//	Download SourceTree to access GitHub:
//		www.sourcetreeapp.com
//	Good intro to perspective transformations, etc:
//	www.scratchapixel.com
//	www.scratchapixel.com/old/lessons/3d-advanced-lessons/perspective-and-orthographic-projection-matrix/perspective-projection-matrix/

#include <cstdio>
#include "IntroToOpenGL.h"
#include "CameraAndProjections.h"
#include "RenderingGeometry.h"
#include "Texturing.h"
#include "Lighting.h"
#include "AdvancedTexturing.h"
#include "Quaternions.h"

int main()
{
//	IntroToOpenGL	app;
//	CameraAndProjections	app;
//	RenderingGeometry	app;
//	Texturing app;
//	Lighting app;
//	AdvancedTexturing app;
	Quaternions	app;

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
