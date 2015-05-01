#pragma once
#define GLM_SWIZZLE
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include<vector>
#include "Agent.h"
#include "Turret.h"
#include "Food.h"
#include "LinearZone.h"

using namespace std;

const int MAX_AGENTS = 1;
const int MAX_TURRETS = 4;
const int MAX_FOOD = 4;
class Camera;

class NNProject
{
public:
	Agent agents[MAX_AGENTS];
	Turret turrets[MAX_TURRETS];
	Food foods[MAX_FOOD];
	LinearZone linearZone;
	NNProject();
	virtual ~NNProject();

	virtual bool	onCreate(int argc, char* argv[]);
	virtual void	onUpdate(float deltaTime);
	virtual void	onDraw();
	virtual void	onDestroy();

protected:
	void setUpSimulation();
	void setUpAgents();
	void setUpTurrets();
	void setUpFood();
	void setUpSimpleLinearZone();
	void updateAgents(float delta);
	void drawAgents();
	void addAgentGizmos();
	void addTurretWidgets();
	void addFoodWidgets();
	void checkAgentDamage(Agent* agent);
	void checkAgentFood(Agent* agent);


	float simulateTurret(glm::vec2&, float, Agent* agent);
	bool GLFWMouseButton1Down;
};

