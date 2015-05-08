//#include <GLFW/glfw3.h>
#include "BasicNetworking.h"

int main()
{
	
	BaseApplication* app = new BasicNetworkingApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}
