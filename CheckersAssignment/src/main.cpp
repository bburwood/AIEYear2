//	need to go through and add the header comments to the files

#include <cstdio>
#include "Checkers.h"

int main()
{
	Checkers	app;

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
