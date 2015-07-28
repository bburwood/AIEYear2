#include <cstdio>
#include "CustomPhysics.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main()
{
	/* initialize random seed: */
	srand(time(NULL));

	CustomPhysics	app;

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
