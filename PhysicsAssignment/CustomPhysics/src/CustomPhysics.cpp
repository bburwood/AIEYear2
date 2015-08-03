#include "CustomPhysics.h"
#include "Gizmos.h"
#include "Utility.h"
#include <iostream>

/*
Notes:
Custom engine
	*	class diagram
	*	2d/3d
	*	Pool game would work - objects interact correctly using forces and collision reactions
	*	spring system - could do the pockets with 2d cloth/rope - maybe make the balls bounce back into the table with spring cloth/rope for the pockets
PhysX:
	*	Static and dynamic
	*	Ragdolls
	*	trigger volume
	*	particle system / soft body
	*	player controller
*/

using namespace std;

//	define some basic colours to use for drawing
vec4	white(1);
vec4	black(0, 0, 0, 1);
vec4	blue(0, 0, 1, 1);
vec4	yellow(1, 1, 0, 1);
vec4	green(0, 1, 0, 1);
vec4	red(1, 0, 0, 1);

CustomPhysics::CustomPhysics()
{

}

CustomPhysics::~CustomPhysics()
{

}


bool	CustomPhysics::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_fTotalTime = 0.0f;
	m_fTimer = 0.0f;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(BUFFER_WIDTH, BUFFER_HEIGHT);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.1f, 0.1f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
	m_bDrawGizmos = true;
	m_bDrawGrid = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(-1, 10, 0.00001f), vec3(-1, 0.000001f, 0.000001f), glm::radians(50.0f), (float)BUFFER_WIDTH / (float)BUFFER_HEIGHT, 0.1f, 3000.0f);
	m_FlyCamera.SetSpeed(25.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("GeneralStuff");	//	must be a single word (no spaces) if you want to be able to resize it
	TwDefine(" GeneralStuff size='280 300' "); // resize bar
	TwAddSeparator(m_bar, "Misc Data", "");
	//TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_vLightDir, "label='Light Direction'");
	//TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR4F, &m_vLightColour, "");
	TwAddVarRW(m_bar, "Ambient Colour", TW_TYPE_COLOR4F, &m_vAmbientLightColour, "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRW(m_bar, "Draw Gizmos Grid", TW_TYPE_BOOL8, &m_bDrawGrid, "");
	TwAddVarRW(m_bar, "Camera Speed", TW_TYPE_FLOAT, &m_FlyCamera.m_fSpeed, "min=1 max=250 step=1");
	TwAddVarRW(m_bar, "Physics Update FPS", TW_TYPE_FLOAT, &m_fPhysicsFrameRate, "min=10 max=250 step=1");
	TwAddVarRW(m_bar, "Cue Sensitivity", TW_TYPE_FLOAT, &m_fCueSpeed, "min=50 max=500 step=1");
	TwAddVarRO(m_bar, "Rendering FPS", TW_TYPE_FLOAT, &m_fFPS, "");

	m_fPhysicsFrameRate = 120.0f;
	m_fPhysicsUpdateTimout = 1.0f / m_fPhysicsFrameRate;
	m_fPhysicsUpdateTimer = 0.0f;
	m_fCueSpeed = 500.0f;

	m_vAmbientLightColour = vec4(0.001f, 0.001f, 0.001f, 1.0f);
	//m_vLightColour = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//m_vLightDir = glm::normalize(vec3(-0.10f, -0.85f, 0.5f));

	SetupPoolGame();

	cout << "CustomPhysics startup complete!\n";
	return true;
}

void	CustomPhysics::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	CustomPhysics::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		//ReloadShader();
	}
	Gizmos::clear();
	float	dT = (float)glfwGetTime();
	if (dT > 0.1f)
	{
		dT = 0.1f;
	}
	glfwSetTime(0.0f);
	m_fTimer += dT;
	m_fTotalTime += dT;
	m_fFPS = (float)(1.0f / dT);
	m_fPhysicsUpdateTimout = 1.0f / m_fPhysicsFrameRate;
	if (physicsScene->timeStep != m_fPhysicsUpdateTimout)
	{
		//	if the desired frame rate has changed, then update it in the physics engine
		physicsScene->timeStep = m_fPhysicsUpdateTimout;
	}
	m_fPhysicsUpdateTimer += dT;

	/////////////////////////////////////////////////
	static glm::vec2	vCMToAnchor = glm::vec2(0);
	static bool	bGrabbed = false;
	//static bool	bAiming = false;

	SphereClass*	pSphere = (SphereClass*)physicsScene->actors[0];
	/*
	BoxClass*	box1 = (BoxClass*)physicsScene->actors[0];
	float	fSpeed = 75.0f;

	if (glfwGetKey(m_window, GLFW_KEY_LEFT))
	{
		box1->velocity.x -= dT * fSpeed;
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT))
	{
		box1->velocity.x += dT * fSpeed;
	}
	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		box1->velocity.y += dT * fSpeed;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		box1->velocity.y -= dT * fSpeed;
	}
	fSpeed = 2.0f;
	if (glfwGetKey(m_window, GLFW_KEY_Q))
	{
		box1->rotation2D += dT * fSpeed;
	}
	if (glfwGetKey(m_window, GLFW_KEY_E))
	{
		box1->rotation2D -= dT * fSpeed;
	}
	*/
	double	mouseX, mouseY;
	glfwGetCursorPos(m_window, &mouseX, &mouseY);
	vec2	vMouseWorld;	// = m_FlyCamera.PickAgainstPlane(mouseX, mouseY, vec4(0, 1, 0, 0));
	vMouseWorld = GetWorldMouse();
	glm::vec2	vAnchor(0);
	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
	{
		if (pSphere->IsPointOver(vMouseWorld))
		{
			cout << "Mouse world pos: " << vMouseWorld.x << " / " << vMouseWorld.y << "\n";
			cout << "Circle world pos: " << pSphere->position.x << " / " << pSphere->position.y << "\n";
		}
		if (!bGrabbed)
		{
			//cout << "Mouse world pos: " << vMouseWorld.x << " / " << vMouseWorld.y << "\n";
			//cout << "Circle world pos: " << pSphere->position.x << " / " << pSphere->position.y << "\n";
			//	if the mouse is over the sphere
			if (pSphere->IsPointOver(vMouseWorld))
			{
				cout << "Mouse world pos: " << vMouseWorld.x << " / " << vMouseWorld.y << "\n";
				cout << "Circle world pos: " << pSphere->position.x << " / " << pSphere->position.y << "\n";
				//	store anchor point
				vCMToAnchor = vMouseWorld - pSphere->position;
				float	fSinTheta = sinf(-pSphere->rotation2D);
				float	fCosTheta = cosf(-pSphere->rotation2D);
				vCMToAnchor = glm::vec2(fCosTheta * vCMToAnchor.x - fSinTheta * vCMToAnchor.y,
					fSinTheta * vCMToAnchor.x + fCosTheta * vCMToAnchor.y);
				//	set grabbed to true
				bGrabbed = true;
				//bAiming = true;
			}
		}
		else
		{
			//	compute anchor point
			float	fSinTheta = sinf(pSphere->rotation2D);
			float	fCosTheta = cosf(pSphere->rotation2D);
			glm::vec2 vRotLocalPos = glm::vec2(fCosTheta * vCMToAnchor.x - fSinTheta * vCMToAnchor.y,
				fSinTheta * vCMToAnchor.x + fCosTheta * vCMToAnchor.y);
			glm::vec2	vTempAnchor = pSphere->position + vRotLocalPos;

			//	add force at anchor point towards the mouse - not for pool
			//box1->applyForceAtPoint(vMouseWorld - vAnchor, vAnchor);
			Gizmos::add2DLine(vTempAnchor, vMouseWorld, glm::vec4(1, 1, 0, 1));
		}
	}
	else
	{
		if ((glfwGetMouseButton(m_window, 0) == GLFW_RELEASE) && bGrabbed)
		{
			//	compute anchor point
			float	fSinTheta = sinf(pSphere->rotation2D);
			float	fCosTheta = cosf(pSphere->rotation2D);
			glm::vec2 vRotLocalPos = glm::vec2(fCosTheta * vCMToAnchor.x - fSinTheta * vCMToAnchor.y,
				fSinTheta * vCMToAnchor.x + fCosTheta * vCMToAnchor.y);
			vAnchor = pSphere->position + vRotLocalPos;

			//	add force at anchor point towards the mouse
			pSphere->applyForceAtPoint(-m_fCueSpeed * (vMouseWorld - vAnchor), vAnchor);
			Gizmos::add2DLine(vAnchor, vMouseWorld, glm::vec4(1, 1, 0, 1));
			vCMToAnchor = glm::vec2(0);
			vAnchor = glm::vec2(0);
			bGrabbed = false;
			//bAiming = false;
		}
	}


	if (m_fPhysicsUpdateTimer > m_fPhysicsUpdateTimout)
	{
		//	time to update the physics engine
		physicsScene->upDate();

		//	lastly, reset the m_fPhysicsUpdateTimer
		m_fPhysicsUpdateTimer -= m_fPhysicsUpdateTimout;	//	rather than reset to zero, this preserves any excess above the timout amount
	}

	m_FlyCamera.update(dT);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	CustomPhysics::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Application::draw();



	if (m_bDrawGizmos)
	{
		physicsScene->upDateGizmos();
		if (m_bDrawGrid)
		{
			Gizmos::addTransform(mat4(1));	//	adds the red green blue unit vectors to the drawn grid of lines
			float	fYOffset = -0.01f;
			for (int i = 0; i <= 20; ++i)
			{
				Gizmos::addLine(vec3(-10 + i, fYOffset, -10), vec3(-10 + i, fYOffset, 10),
					i == 10 ? white : black);
				Gizmos::addLine(vec3(-10, fYOffset, -10 + i), vec3(10, fYOffset, -10 + i),
					i == 10 ? white : black);
			}
		}
		//Gizmos::draw(m_FlyCamera.GetProjectionView());
		//	now draw 2D gizmos ...
		float AR = BUFFER_WIDTH / (float)BUFFER_HEIGHT;
		Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / AR, 100 / AR, -10.0f, 10.0f));

	}
	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	CustomPhysics::SetupPoolGame()
{
	physicsScene = new DIYPhysicScene();
	physicsScene->collisionEnabled = true;
	physicsScene->timeStep = m_fPhysicsUpdateTimout;
	physicsScene->gravity = glm::vec2(0.0f, 0.0f);

	float	fBallRadius = 2.25f;
	float	fCueBallMass = 2.0f;
	float	fRedBallMass = 0.5f;
	float	fSqrt3 = 1.7320508075688772935274463415059f;
	glm::vec2	vStartTriangleHead(45.0f, 5.0f);

	SphereClass* sphere1;
	sphere1 = new SphereClass(glm::vec2(-25.0f, vStartTriangleHead.y), glm::vec2(0.0f, 0.0f), fBallRadius, fCueBallMass, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	physicsScene->addActor(sphere1);
//	BoxClass*	box1 = new BoxClass(glm::vec2(-8.0f, 30.0f), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, 4.0f, 4.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//	physicsScene->addActor(box1);

	for (int iRow = 0; iRow < 5; ++iRow)
	{
		for (int iNumInRow = 0; iNumInRow <= iRow; ++iNumInRow)
		{
			sphere1 = new SphereClass(glm::vec2(vStartTriangleHead.x + (fBallRadius * iRow * fSqrt3), vStartTriangleHead.y + (fBallRadius * (iRow - (2.0f * iNumInRow)))),
				glm::vec2(0, 0), fBallRadius, fRedBallMass, glm::vec4(1, 0, 0, 1));
			physicsScene->addActor(sphere1);
		}
	}
//	sphere1 = new SphereClass(glm::vec2(-20.0f, vStartTriangleHead.y + 5.0f), glm::vec2(0.0f, 0.0f), fBallRadius, 10.5f, glm::vec4(0, 0, 1, 1));
//	sphere1->bIsStatic = true;
//	physicsScene->addActor(sphere1);

	//	add the boxes for the pool table
	float	fTop = 40.0f;
	float	fBottom = -30.0f;
	float	fLeft1 = -10.0f;
	float	fLeftEdge = -45.0f;
	float	fEdgeY = 5.0f;
	float	fRight1 = 50.0f;
	float	fRightEdge = 85.0f;
	float	fBoxLength = 25.0f;
	float	fBoxThickness = 4.0f;
	BoxClass*	box = new BoxClass(glm::vec2(fLeft1, fTop), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxLength, fBoxThickness, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);
	box = new BoxClass(glm::vec2(fLeft1, fBottom), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxLength, fBoxThickness, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);
	box = new BoxClass(glm::vec2(fRight1, fTop), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxLength, fBoxThickness, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);
	box = new BoxClass(glm::vec2(fRight1, fBottom), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxLength, fBoxThickness, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);
	box = new BoxClass(glm::vec2(fLeftEdge, fEdgeY), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxThickness, 25.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);
	box = new BoxClass(glm::vec2(fRightEdge, fEdgeY), glm::vec2(0.0f, 0.0f), 0.0f, 10.0f, fBoxThickness, 25.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	box->bIsStatic = true;
	physicsScene->addActor(box);

	float	fLengthPlus = 1.001f * fBoxLength;
	float	fThicknessPlus = 1.01f * fBoxThickness;
	int		iSegments = 6;
	CreateChain(glm::vec2(fLeftEdge + fThicknessPlus, fEdgeY + fLengthPlus), glm::vec2(fLeft1 - fLengthPlus, fTop - fThicknessPlus), iSegments, 110.0f, 0.2f);
	CreateChain(glm::vec2(fLeftEdge + fThicknessPlus, fEdgeY - fLengthPlus), glm::vec2(fLeft1 - fLengthPlus, fBottom + fThicknessPlus), iSegments, 110.0f, 0.2f);
	CreateChain(glm::vec2(fLeft1 + fLengthPlus, fTop), glm::vec2(fRight1 - fLengthPlus, fTop), iSegments, 110.0f, 0.2f);
	CreateChain(glm::vec2(fLeft1 + fLengthPlus, fBottom), glm::vec2(fRight1 - fLengthPlus, fBottom), iSegments, 110.0f, 0.2f);
	CreateChain(glm::vec2(fRight1 + fLengthPlus, fTop - fThicknessPlus), glm::vec2(fRightEdge - fThicknessPlus, fEdgeY + fLengthPlus), iSegments, 110.0f, 0.2f);
	CreateChain(glm::vec2(fRight1 + fLengthPlus, fBottom + fThicknessPlus), glm::vec2(fRightEdge - fThicknessPlus, fEdgeY - fLengthPlus), iSegments, 110.0f, 0.2f);

	//	testing with planes ...
	PlaneClass* plane = new PlaneClass(glm::vec2(0.25f, 1.0f), -55.0f);
	physicsScene->addActor(plane);

	PlaneClass* plane2 = new PlaneClass(glm::vec2(-0.25f, 1.0f), -55.0f);
	physicsScene->addActor(plane2);
}

void	CustomPhysics::CreateChain(glm::vec2 a_vStartPos, glm::vec2 a_vEndPos, int a_iSegments, float a_fSpringK, float a_fSpringDamping)
{
	SphereClass**	chain = new SphereClass*[a_iSegments + 1];
	Joint**	joints = new Joint*[a_iSegments];
	glm::vec2	vSegmentLength = (a_vEndPos - a_vStartPos) / (float)(a_iSegments);
	//	first make the spheres
	float	fSphereMass = 0.2f;
	float	fSphereRadius = 0.0025f;
	glm::vec2	vCurrentPos = a_vStartPos;
	for (int i = 0; i < a_iSegments + 1; ++i)
	{
		//float	fDistance = fStartChainX + (fSeparation * i);
		chain[i] = new SphereClass(vCurrentPos, glm::vec2(0), fSphereRadius, fSphereMass, glm::vec4(1, 1, 1, 1));
		chain[i]->dynamicFriction = 0.2f;
		chain[i]->staticFriction = 0.2f;
		physicsScene->addActor(chain[i]);
		vCurrentPos += vSegmentLength;
	}
	//	increase the mass of the corner sphere's in the grid
	chain[0]->bIsStatic = true;
	chain[a_iSegments]->bIsStatic = true;

	//	now make the joints between them
	float	fRestDistance = vSegmentLength.length() * 0.5f;
	for (int i = 0; i < a_iSegments; ++i)
	{
		joints[i] = new SpringJoint(chain[i], chain[i + 1], a_fSpringK, a_fSpringDamping, fRestDistance, glm::vec4(1, 1, 1, 1));
		physicsScene->addJoint(joints[i]);
	}
	delete[]	chain;
	delete[]	joints;
}

glm::vec2	CustomPhysics::GetWorldMouse()
{
	double	mouseX, mouseY;
	glfwGetCursorPos(m_window, &mouseX, &mouseY);
	mouseX /= (double)BUFFER_WIDTH;
	mouseY /= (double)BUFFER_HEIGHT;
	mouseX -= 0.5;
	mouseX *= 200.0;
	mouseY -= 0.5;
	mouseY *= -200.0 * ((double)BUFFER_HEIGHT / (double)BUFFER_WIDTH);
	return glm::vec2((float)mouseX, (float)mouseY);
}
