//	Download SourceTree to access GitHub:
//		www.sourcetreeapp.com
//	Good intro to perspective transformations, etc:
//	www.scratchapixel.com
//	www.scratchapixel.com/old/lessons/3d-advanced-lessons/perspective-and-orthographic-projection-matrix/perspective-projection-matrix/

#include <cstdio>
#include "IntroToOpenGL.h"
#include "CameraAndProjections.h"
#include "RenderingGeometry.h"

int main()
{
//	IntroToOpenGL	app;
//	CameraAndProjections	app;
	RenderingGeometry	app;

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
