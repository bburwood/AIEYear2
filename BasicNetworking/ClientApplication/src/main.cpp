//#include <GLFW/glfw3.h>
#include "BasicNetworking.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main()
{
	/* initialize random seed: */
	srand(time(NULL));

	BaseApplication* app = new BasicNetworkingApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}
