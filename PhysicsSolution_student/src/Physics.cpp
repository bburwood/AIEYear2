#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "glm/gtc/quaternion.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))


class AIEAllocator : public PxAllocatorCallback
{
public:
	AIEAllocator();
	virtual	~AIEAllocator(){};
	virtual	void*	allocate(size_t bytes, const char* type_name, const char* filesname, int line)
	{
		void*	ptr = _aligned_malloc(bytes, 16);
		return ptr;
	}

	virtual	void	deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}

private:

};

AIEAllocator::AIEAllocator()
{
}

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }
	/* initialize random seed: */
	srand(time(NULL));

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(0, 60, 120), vec3(0, 40, 80), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_fTimer = 0.0f;
	m_bSpheresCreated = false;
	m_iNextSphereToFire = 0;
	m_fFiringTimer = 0.0f;
	m_fFiringInterval = c_fSphereFiringInterval;
	m_fFiringSpeed = c_fSphereFiringSpeed;

    setupPhysx();
    setupTutorial1();
	setupVisualDebugger();

	m_oTank.Init(&m_camera, m_pPhysicsScene);	//	initialise the tank!

	return true;
}

void	Physics::setupVisualDebugger()
{
	//	check if the PVDConnection manager is available on this platform - will NOT be in release
	if (m_pPhysics->getPvdConnectionManager() == 0)
	{
		return;
	}
	const	char*	pvdHostIP = "127.0.0.1";
	int	iPort = 5425;
	unsigned int	uiTimeout = 100;
	PxVisualDebuggerConnectionFlags	flags = PxVisualDebuggerExt::getAllConnectionFlags();
	auto	theConnection = PxVisualDebuggerExt::createConnection(m_pPhysics->getPvdConnectionManager(), pvdHostIP, iPort, uiTimeout, flags);
}

void Physics::setupTutorial1()
{
	//	add a plane to the scene
	PxTransform	transform = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat((float)PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f).getNormalized()) * PxQuat((float)PxHalfPi * 0.20f, PxVec3(0.0f, -1.0f, 0.0f).getNormalized()));
	PxRigidStatic*	plane = PxCreateStatic(*m_pPhysics, transform, PxPlaneGeometry(), *m_pPhysicsMaterial);
	m_pPhysicsScene->addActor(*plane);

	//	add a box to the scene
	float	density = 5.0f;
	PxBoxGeometry	box(1.0f, 1.0f, 1.0f);
	PxTransform		boxTransform(PxVec3(-3.0f, 5.0f, 3.0f));
	m_pBoxActor = PxCreateDynamic(*m_pPhysics, boxTransform, box, *m_pPhysicsMaterial, density);
	m_pPhysicsScene->addActor(*m_pBoxActor);


	//	now create lots of boxes!!! ...
	for (int i = 0; i < c_iNumBoxes; ++i)
	{
		PxBoxGeometry	tBox(c_fBoxSize, c_fBoxSize, c_fBoxSize);
		PxTransform		tBoxTransform(PxVec3((float)(rand() % 40) - 20.0f, 50.0f + c_fBoxSize + (float)i * 0.2f, (float)(rand() % 40) - 20.0f));
		m_aBoxes[i] = PxCreateDynamic(*m_pPhysics, tBoxTransform, tBox, *m_pPhysicsMaterial, density);
		m_pPhysicsScene->addActor(*m_aBoxes[i]);
		//	now make their colours
		m_aColours[i] = vec4((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, 1.0f);
	}
}

void	Physics::CreateSpheres()
{
	//	now create lots of spheres!!! ...
	float	density = 200.0f;
	for (int i = 0; i < c_iNumSpheres; ++i)
	{
		PxSphereGeometry	tSphere(c_fSphereRadius);
		PxTransform		tSphereTransform(PxVec3((float)(rand() % 20) - 10.0f, 40.0f + c_fSphereRadius + (float)i * 0.2f, (float)(rand() % 20) - 10.0f));
		m_aSpheres[i] = PxCreateDynamic(*m_pPhysics, tSphereTransform, tSphere, *m_pPhysicsMaterial, density);
		m_pPhysicsScene->addActor(*m_aSpheres[i]);
		//	now make their colours
		m_aSphereColours[i] = vec4((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, 1.0f);
	}
	m_bSpheresCreated = true;
}

void Physics::setupPhysx()
{
	m_DefaultFilterShader = PxDefaultSimulationFilterShader;
	PxAllocatorCallback*	pMyCallback = new AIEAllocator();

	m_pPhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *pMyCallback, m_DefaultErrorCallback);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*m_pPhysics);

	PxSceneDesc	oSceneDesc(m_pPhysics->getTolerancesScale());
	oSceneDesc.gravity = PxVec3(0.0f, -27.0f, 0.0f);
	oSceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	oSceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(8);	//	only use 1 thread - can change this
	m_pPhysicsScene = m_pPhysics->createScene(oSceneDesc);

	m_pPhysicsMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
}

void Physics::shutdown()
{
	//	now clean up the PhysX system
	m_pPhysicsScene->release();
	m_pPhysics->release();
	m_pPhysicsFoundation->release();

    Gizmos::destroy();
    Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);
	m_fTimer += dt;
	if ((m_fTimer > 5.0f) && !m_bSpheresCreated)
	{
		CreateSpheres();
	}

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

	if(m_bSpheresCreated)
	{
		m_fFiringTimer += dt;
		if ((glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) && (m_fFiringTimer > m_fFiringInterval))
		{
			//	fire the next sphere in the array in the forward direction of the camera, from just below the camera
			vec3	vCamPos = m_camera.world[3].xyz;
			vec3	vForward = (m_camera.world[2].xyz) * (-1.0f);
			vCamPos += 2.0f * vForward;
			vCamPos.y -= 3.0f;
			PxTransform		tSphereTransform(PxVec3(vCamPos.x, vCamPos.y, vCamPos.z));
			m_aSpheres[m_iNextSphereToFire]->setGlobalPose(tSphereTransform);
			vForward *= m_fFiringSpeed;
			m_aSpheres[m_iNextSphereToFire]->setLinearVelocity(PxVec3(vForward.x, vForward.y, vForward.z));
			m_iNextSphereToFire = (m_iNextSphereToFire + 1) % c_iNumSpheres;
			m_fFiringTimer = 0.0f;
		}
	}

	m_pPhysicsScene->simulate(dt > 0.0333f ? 0.0333f : dt);	//	limit the maximum time step

	while (m_pPhysicsScene->fetchResults() == false)
	{

	}

	m_oTank.update(dt);

	//	get the box transform
	PxTransform	boxTransform = m_pBoxActor->getGlobalPose();
	//	get its position
	vec3	vBoxPosition(boxTransform.p.x, boxTransform.p.y, boxTransform.p.z);
	//	get its rotation
	glm::quat	qBoxRotation(boxTransform.q.w, boxTransform.q.x, boxTransform.q.y, boxTransform.q.z);
	//	add it as a gizmo
	glm::mat4	rot(qBoxRotation);
	Gizmos::addAABBFilled(vBoxPosition, vec3(1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), &rot);

	//	now draw lots of boxes!!! ...
	for (int i = 0; i < c_iNumBoxes; ++i)
	{
		boxTransform = m_aBoxes[i]->getGlobalPose();
		vBoxPosition.x = boxTransform.p.x;
		vBoxPosition.y = boxTransform.p.y;
		vBoxPosition.z = boxTransform.p.z;
		qBoxRotation.x = boxTransform.q.x;
		qBoxRotation.y = boxTransform.q.y;
		qBoxRotation.z = boxTransform.q.z;
		qBoxRotation.w = boxTransform.q.w;
		rot = glm::mat4(qBoxRotation);
		Gizmos::addAABBFilled(vBoxPosition, vec3(c_fBoxSize, c_fBoxSize, c_fBoxSize), m_aColours[i], &rot);
	}

	if (m_bSpheresCreated)
	{
		//	now draw lots of spheres!!! ...
		for (int i = 0; i < c_iNumSpheres; ++i)
		{
			boxTransform = m_aSpheres[i]->getGlobalPose();
			vBoxPosition.x = boxTransform.p.x;
			vBoxPosition.y = boxTransform.p.y;
			vBoxPosition.z = boxTransform.p.z;
			qBoxRotation.x = boxTransform.q.x;
			qBoxRotation.y = boxTransform.q.y;
			qBoxRotation.z = boxTransform.q.z;
			qBoxRotation.w = boxTransform.q.w;
			rot = glm::mat4(qBoxRotation);
			Gizmos::addSphere(vBoxPosition, c_fSphereRadius, 5, 6, m_aSphereColours[i], &rot);
		}
	}

    m_camera.update(dt);

    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_oTank.draw();
    Gizmos::draw(m_camera.proj, m_camera.view);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}