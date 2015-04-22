#include "UtilitySystemApplication.h"
#include <GLFW/glfw3.h>

#include "UtilityMaths.h"
using namespace UtilitySystem::UtilityMath;

int main() {
	
	BaseApplication* app = new UtilitySystemApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}