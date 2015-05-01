#include "NNProject.h"
#include <iostream>
using namespace std;

int gScreenWidth;
int gScreenHeight;

NNProject::NNProject()
{

}

NNProject::~NNProject()
{

	
}

bool NNProject::onCreate(int argc, char* argv[])
{
	// initialise the Gizmos helper class
	Gizmos::create(0xffff, 0xffff, 0xffff, 0xffff);

	// set the colour the window is cleared to
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	GLFWMouseButton1Down = false;

	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	setUpSimulation();
	return true;
}

void NNProject::setUpSimulation()
{
	setUpAgents();
	setUpSimpleLinearZone();
//	setUpTurrets();
	//setUpFood();

}

void NNProject::onDestroy()
{
	Gizmos::destroy();
}

void NNProject::setUpSimpleLinearZone()
{
	glm::vec2 centre(600, 400);
	float rotation = 1.5;
	linearZone = LinearZone(centre, rotation);
}

void NNProject::setUpAgents()
{
	srand((unsigned int)time(NULL));
	for (int index = 0; index < MAX_AGENTS;index++)
	{
		glm::vec2 startPos;
		startPos.x =  (float)(rand() % gScreenWidth);
		startPos.y =  (float)(rand() %2 * gScreenHeight);
		float size = 30;
		float facing=  44/7.0f * ((rand()%1000)/1000.0f);
		agents[index].setup(startPos, size, glm::vec4(1, 1, 1, 1), facing);
	}
}

void NNProject::onUpdate(float deltaTime)
{
	// clear all gizmos from last frame
	Gizmos::clear();

	updateAgents(deltaTime);
	linearZone.addGizmo();
	addAgentGizmos();
	addTurretWidgets();
	addFoodWidgets();

}



void NNProject::onDraw()
{
	// clear the back-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, (float)width, 0, (float)height, -1.0f, 1.0f));
}


void NNProject::updateAgents(float delta)
{
	for (int index = 0; index < MAX_AGENTS; index++)
	{
		agents[index].update(delta);
		checkAgentDamage(&agents[index]);
		checkAgentFood(&agents[index]);
	}
}

void NNProject::checkAgentDamage(Agent* agent)
{
	float damage = 0;
	damage += linearZone.checkRange(agent->getPosition());
	for (auto turret : turrets)
	{
		damage += turret.checkRange(agent->getPosition());
	}
	agent->hurtAgent(damage);
}

void NNProject::checkAgentFood(Agent* agent)
{
	float foodFound = 0;
	for (auto food : foods)
	{
		foodFound += food.checkRange(agent->getPosition());
	}
	agent->feedAgent(foodFound);
}

void NNProject::setUpTurrets()
{
	turrets[0] = Turret(glm::vec2(400,300),200);
//	turrets[1] = Turret(glm::vec2(1100, 300), 200); //second turret for next experiment

}

void NNProject::setUpFood()
{
	foods[0] = Food(glm::vec2(900, 50), 75);
	foods[1] = Food(glm::vec2(600,600), 75);
}

float NNProject::simulateTurret(glm::vec2& centre, float range, Agent* agent)
{
	glm::vec2 displacment = agent->getPosition() - centre;
	float distance = glm::length(displacment);
	if (distance<range)
	{
		return true;
	}
	return false;
}

void NNProject::drawAgents()
{
	for (int index = 0; index < MAX_AGENTS; index++)
	{
		agents[index].draw();
	}
}

void NNProject::addAgentGizmos()
{
	for (auto agent : agents)
	{
		agent.addGizmo();
	}
}

void NNProject::addTurretWidgets()
{
	for (auto turret :turrets)
	{
		turret.addGizmo();
	}
}


void NNProject::addFoodWidgets()
{
	for (auto food : foods)
	{
		food.addGizmo();
	}
}