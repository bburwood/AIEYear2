#include <gl_core_4_4.h>
#include <glfw3.h>
#include <aieutilities/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include "DIYPhysicsEngine.h"

void DIYPhysicsRocketSetup();
void upDate2DPhysics(float delta);
void DIYPhysicsCollisionTutorial();
void draw2DGizmo();
void onUpdateRocket(float deltaTime);

DIYPhysicScene* physicsScene;
SphereClass* rocket;
GLFWwindow* window;

int main()
{
	if (glfwInit() == false)
	{
		return -1;
	}

//	DIYPhysicsRocketSetup();
	DIYPhysicsCollisionTutorial();
	window = glfwCreateWindow(1080, 720, "Physics 2D", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	Gizmos::create();

	glm::mat4 view = glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);

	float angle = 0;
	float angle2 = 0;
	float angle3 = 0;
	glm::vec3 loc2 = glm::vec3(8, 0, 0);
	glm::vec3 loc3 = glm::vec3(4, 0, 0);
	float prevTime = 0;

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		// grab the time since the application started (in seconds)
		float time = (float)glfwGetTime();

		// calculate a delta time
		float deltaTime = time - prevTime;
		prevTime = time;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.102f, 0.489f, 0.866f, 1);
		glEnable(GL_DEPTH_TEST);

		Gizmos::clear();
		upDate2DPhysics(deltaTime);
//		Gizmos::addTransform(glm::mat4(1));

		Gizmos::draw(projection * view);
		draw2DGizmo();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void draw2DGizmo()
{
	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	float AR = width / (float)height;
	Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / AR, 100 / AR, -1.0f, 1.0f));
}

glm::vec2	GetWorldMouse()
{
	int	iWidth = 0, iHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &iWidth, &iHeight);
	double	mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	mouseX /= iWidth;
	mouseY /= iHeight;
	mouseX -= 0.5;
	mouseX *= 200.0f;
	mouseY -= 0.5;
	mouseY *= -200.0f * ((float)iHeight / (float)iWidth);
	return glm::vec2((float)mouseX, (float)mouseY);
}

void upDate2DPhysics(float delta)
{
	static glm::vec2	vCMToAnchor = glm::vec2(0);
	static bool	bGrabbed = false;

	BoxClass*	box1 = (BoxClass*)physicsScene->actors[0];
	float	fSpeed = 25.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		box1->velocity.x -= delta * fSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		box1->velocity.x += delta * fSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		box1->velocity.y += delta * fSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		box1->velocity.y -= delta * fSpeed;
	}
	fSpeed = 2.0f;
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		box1->rotation2D += delta * fSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		box1->rotation2D -= delta * fSpeed;
	}

	if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
	{
		if (!bGrabbed)
		{
			//	if the mouse is over the box
			if (box1->IsPointOver(GetWorldMouse()))
			{
				//	store anchor point
				vCMToAnchor = GetWorldMouse() - box1->position;
				float	fSinTheta = sinf(-box1->rotation2D);
				float	fCosTheta = cosf(-box1->rotation2D);
				vCMToAnchor = glm::vec2(fCosTheta * vCMToAnchor.x - fSinTheta * vCMToAnchor.y,
										fSinTheta * vCMToAnchor.x + fCosTheta * vCMToAnchor.y);
				//	set grabbed to true
				bGrabbed = true;
			}
		}
		else
		{
			//	compute anchor point
			float	fSinTheta = sinf(box1->rotation2D);
			float	fCosTheta = cosf(box1->rotation2D);
			glm::vec2 vRotLocalPos = glm::vec2(	fCosTheta * vCMToAnchor.x - fSinTheta * vCMToAnchor.y,
												fSinTheta * vCMToAnchor.x + fCosTheta * vCMToAnchor.y);
			glm::vec2	vAnchor = box1->position + vRotLocalPos;

			//	add force at anchor point towards the mouse
			box1->applyForceAtPoint(GetWorldMouse() - vAnchor, vAnchor);
			Gizmos::add2DLine(vAnchor, GetWorldMouse(), glm::vec4(1, 1, 0, 1));
		}
	}
	else
	{
		bGrabbed = false;
	}
	physicsScene->upDate();
	physicsScene->upDateGizmos();
	//onUpdateRocket(delta);
}

void DIYPhysicsRocketSetup()
{

	//note - collision detection must be disabled in the physics engine for this to work.
	physicsScene = new DIYPhysicScene();
	physicsScene->collisionEnabled = false;
	physicsScene->gravity = glm::vec2(0, -.2);
	physicsScene->timeStep = .001f;
	rocket = new SphereClass(glm::vec2(-60, -20), glm::vec2(0, 0), 6.0f, 25, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(rocket);
}

void	SetupPoolGame()
{
	float	fBallRadius = 2.50f;
	float	fSqrt3 = 1.7320508075688772935274463415059f;
	glm::vec2	vStartTriangleHead(20.0f, 0.0f);

//	BoxClass*	box1 = new BoxClass(glm::vec2(-8, 30), glm::vec2(0, 0), 0.0f, 10, 4, 4, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//	physicsScene->addActor(box1);

	SphereClass* sphere1;
	sphere1 = new SphereClass(glm::vec2(-60.0f, vStartTriangleHead.y), glm::vec2(35.0f, 2.05f), fBallRadius, 10.5f, glm::vec4(1, 1, 1, 1));
	physicsScene->addActor(sphere1);
	for (int iRow = 0; iRow < 5; ++iRow)
	{
		for (int iNumInRow = 0; iNumInRow <= iRow; ++iNumInRow)
		{
			sphere1 = new SphereClass(glm::vec2(vStartTriangleHead.x + (fBallRadius * iRow * fSqrt3), vStartTriangleHead.y + (fBallRadius * (iRow - (2.0f * iNumInRow)))),
				glm::vec2(0, 0), fBallRadius, 0.5f, glm::vec4(1, 0, 0, 1));
			physicsScene->addActor(sphere1);
		}
	}

}

void DIYPhysicsCollisionTutorial()
{

	//note - collision detection must be disabled in the physics engine for this to work.
	physicsScene = new DIYPhysicScene();
	physicsScene->collisionEnabled = true;
	physicsScene->timeStep = 0.001f;
	physicsScene->gravity = glm::vec2(0, -1);

	SetupPoolGame();
	/*
	BoxClass*	box1 = new BoxClass(glm::vec2(-8, 30), glm::vec2(0, 0), 0.0f, 10, 4, 4, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	physicsScene->addActor(box1);
	BoxClass*	box2 = new BoxClass(glm::vec2(-14, 30), glm::vec2(0, 0), 0.0f, 10, 4, 4, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	physicsScene->addActor(box2);
	BoxClass*	box3 = new BoxClass(glm::vec2(-24, 20), glm::vec2(0, 0), 0.0f, 10, 4, 4, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	physicsScene->addActor(box3);

	SphereClass* sphere1 = new SphereClass(glm::vec2(-70, 25), glm::vec2(0, 0), 8.0f, 8, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere1);
	SphereClass* sphere1a = new SphereClass(glm::vec2(-35, 40), glm::vec2(0, 0), 7.0f, 7, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere1a);


	SphereClass* sphere2 = new SphereClass(glm::vec2(-45, 15), glm::vec2(0, 0), 6.0f, 6, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere2);
	SphereClass* sphere2a = new SphereClass(glm::vec2(-55, 30), glm::vec2(0, 0), 5.0f, 5, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere2a);


	SphereClass* sphere3 = new SphereClass(glm::vec2(-35, 10), glm::vec2(0, 0), 4.0f, 4, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere3);
	SphereClass* sphere3a = new SphereClass(glm::vec2(-25, 15), glm::vec2(0, 0), 4.5f, 4.5f, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere3a);

	SphereClass* sphere4 = new SphereClass(glm::vec2(10, 50), glm::vec2(4, -4), 6.0f, 6, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere4);
	SphereClass* sphere4a = new SphereClass(glm::vec2(-5, 0), glm::vec2(0, 0), 7.0f, 7, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere4a);

	SphereClass* sphere5 = new SphereClass(glm::vec2(-10, 15), glm::vec2(0, 0), 7.0f, 7, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere5);
	SphereClass* sphere5a = new SphereClass(glm::vec2(20, 35), glm::vec2(3, -3), 9.0f, 9, glm::vec4(1, 0, 0, 1));
	physicsScene->addActor(sphere5a);
*/
	PlaneClass* plane = new PlaneClass(glm::vec2(0.5f, 1), -50);
	physicsScene->addActor(plane);

	PlaneClass* plane2 = new PlaneClass(glm::vec2(-0.5f, 1), -50);
	physicsScene->addActor(plane2);
//	*/
}


void onUpdateRocket(float deltaTime)
{
	if (rocket != nullptr)
	{
		static float fireCounter = 0;
		fireCounter -= deltaTime;
		if (fireCounter <= 0)
		{
			float exhaustMass = .001f;
			fireCounter = 0.02f;
			SphereClass *exhaust;
			glm::vec2 position = rocket->position;
			if (rocket->mass > exhaustMass)
			{
				rocket->mass -= exhaustMass;
				exhaust = new SphereClass(position, glm::vec2(0, 0), 1, exhaustMass, glm::vec4(0, 1, 0, 1));
				physicsScene->addActor(exhaust);
				exhaust->applyForceToActor(rocket, glm::vec2(1.5, 1.0));
			}
		}
	}
}
