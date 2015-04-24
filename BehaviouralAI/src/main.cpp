#include "UtilitySystemApplication.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "UtilityMaths.h"
using namespace UtilitySystem::UtilityMath;

int main() {
	/* initialize random seed: */
	srand(time(NULL));

	BaseApplication* app = new UtilitySystemApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}