#include "FuzzyLogicProject.h"
#include <iostream>
#include "FuzzyLogicEngine.h"
#include "SimulationObjects.h"

Fuzzy fuzzyEngine;  //not much of an engine but it's a start
using namespace std;

int gScreenWidth;
int gScreenHeight;

FuzzyProject::FuzzyProject()
{

}

FuzzyProject::~FuzzyProject()
{

	
}

bool FuzzyProject::onCreate(int argc, char* argv[])
{
	// initialise the Gizmos helper class
	Gizmos::create(0xffff, 0xffff, 0xffff, 0xffff);

	// set up a camera
	//m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.0f);
	//m_camera->setLookAtFrom(glm::vec3(50, 10, 0), glm::vec3(0, 0, 0));
    
	// set the colour the window is cleared to
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	GLFWMouseButton1Down = false;

	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	FuzzyLogicExample();
	return true;
}

void FuzzyProject::FuzzyLogicExample()
{
	//set up simulation world.
	worldController = new WorldController();
	Agent* simpleAI = new Agent(glm::vec2(500, 400));
	worldController->addObject(simpleAI);
	Water* water1 = new Water(glm::vec2(1000, 100));
	worldController->addObject(water1);
	Cave* cave = new Cave(glm::vec2(200, 100));
	worldController->addObject(cave);
	Food* food = new Food(glm::vec2(300, 600));
	worldController->addObject(food);

	//the following code sets up all the membership functions for the fuzzy sets

	//membership functions for the tiredess set
	fuzzyEngine.tired = new leftShoulderMembershipFunction(0.2f, 0.4f, "tired");
	fuzzyEngine.awake = new TrapezoidFunction(0.2f, 0.4f, 0.6f, 0.8f, "awake");
	fuzzyEngine.superActive = new rightShoulderMembershipFunction(0.6f, 0.8f, "SuperActive");

	//membership functions for the hunger set
	fuzzyEngine.veryHungry = new leftShoulderMembershipFunction(0.2f, 0.4f, "very hungry");
	fuzzyEngine.hungry = new TrapezoidFunction(.2f, .4f, .8f, .9f, "hungry");
	fuzzyEngine.full = new rightShoulderMembershipFunction(.8f, .9f, "full");

	//membership functions for the thirst set
	fuzzyEngine.WeekFromThirsty = new leftShoulderMembershipFunction(0.1f, 0.3f, "week from thirst");
	fuzzyEngine.veryThirsty = new TriangleFunction(0.1f, 0.3f, 0.5f, "very thristy");
	fuzzyEngine.thirsty = new TriangleFunction(0.3f, 0.5f, 0.7f, "thristy");
	fuzzyEngine.notThirsty = new rightShoulderMembershipFunction(.5f, .7f, "not thirsty");

	//membership functions for the distance set
	fuzzyEngine.veryNear = new leftShoulderMembershipFunction(2, 4, "very close");
	fuzzyEngine.mediumRange = new TrapezoidFunction(2, 4, 50, 70, "medium range");
	fuzzyEngine.farAway = new rightShoulderMembershipFunction(50, 70, "far away");

	//membership functions for the desirability set (used for defuzification)
	fuzzyEngine.undesirable = new leftShoulderMembershipFunction(0.3f, 0.5f, "undesirable");
	fuzzyEngine.desirable = new TriangleFunction(0.3f, 0.5f, 0.7f, "desirable");
	fuzzyEngine.veryDesirable = new rightShoulderMembershipFunction(0.5f, 0.7f, "very desirable");
}



void FuzzyProject::onDestroy()
{
	Gizmos::destroy();
}

void FuzzyProject::onUpdate(float deltaTime)
{
	Gizmos::clear();
	worldController->update(deltaTime);
	worldController->draw();
	// update the camera
	//m_camera->update(deltaTime);

	// clear all gizmos from last frame
}



void FuzzyProject::onDraw()
{
	// clear the back-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the gizmos for this frame
	//Gizmos::draw(m_camera->getProjectionView());

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, (float)width, 0, (float)height, -1.0f, 1.0f));
}


