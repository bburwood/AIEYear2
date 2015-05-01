#pragma once
#define GLM_SWIZZLE

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "SimulationObjects.h"
#include <vector>

using namespace std;

class Camera;

class FuzzyProject
{
public:
	FuzzyProject();
	virtual ~FuzzyProject();

	WorldController* worldController;
	bool	onCreate(int argc, char* argv[]);
	void	onUpdate(float deltaTime);
	void	onDraw();
	void	onDestroy();
	
    //Camera*			m_camera;
	
    bool GLFWMouseButton1Down;
	void FuzzyLogicExample();
};

