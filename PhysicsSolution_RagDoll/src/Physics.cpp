#include "Physics.h"
#include "Ragdoll.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

RagDollNode*	ragdollData[] = 
{
	new RagDollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NO_PARENT, 1, 3, 1, 1, "lower spine"),
	new RagDollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "left pelvis"),
	new RagDollNode(PxQuat(0, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "right pelvis"),
	new RagDollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_PELVIS, 5, 2, -1, 1, "L upper leg"),
	new RagDollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_PELVIS, 5, 2, -1, 1, "R upper leg"),
	new RagDollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_UPPER_LEG, 5, 1.75f, -1, 1, "L lower leg"),
	new RagDollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_UPPER_LEG, 5, 1.75, -1, 1, "R lower leg"),
	new RagDollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE, 1, 3, 1, -1, "upper spine"),
	new RagDollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE, 1, 1.5f, 1, 1, "left clavicle"),
	new RagDollNode(PxQuat(0, Z_AXIS), UPPER_SPINE, 1, 1.5f, 1, 1, "right clavicle"),
	new RagDollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE, 1, 1, 1, -1, "neck"),
	new RagDollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK, 1, 3, 1, -1, "Head"),
	new RagDollNode(PxQuat(PxPi, Z_AXIS), LEFT_CLAVICLE, 3, 1.5f, -1, 1, "left upper arm"),
	new RagDollNode(PxQuat(0.3f, Z_AXIS), RIGHT_CLAVICLE, 3, 1.5f, -1, 1, "right upper arm"),
	new RagDollNode(PxQuat(PxPi, Z_AXIS), LEFT_UPPER_ARM, 3, 1, -1, 1, "left lower arm"),
	new RagDollNode(PxQuat(0.3f, Z_AXIS), RIGHT_UPPER_ARM, 3, 1, -1, 1, "right lower arm"),
	nullptr
};


class MyAllocator : public physx::PxAllocatorCallback
{
    virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
    {
        void* new_mem = malloc(size + 32);
        void* result = (char*)new_mem + (16 - ((size_t)new_mem % 16));
        Assert(((size_t)result % 16) == 0);
        *(void**)result = new_mem;
        return (char*)result + 16;
    }

    virtual void deallocate(void* ptr)
    {
        if (ptr)
        {
            void* real_ptr = *(void**)((char*)ptr - 16);
            free(real_ptr);
        }
    }
};

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create(1<<20, 1<<20);


    int screen_width, screen_height;
    glfwGetWindowSize(m_window, &screen_width, &screen_height);

    screen_size = glm::vec2(screen_width, screen_height);


    m_camera = FlyCamera(screen_size.x / screen_size.y, 10.0f);
    m_camera.setLookAt(vec3(20, 15, 20), vec3(0, 10, 0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_fSpawnTimer = cg_fSPAWN_INTERVAL + 0.01f;

    setupPhysx();
    setupTutorial1();
    return true;
}

void Physics::setupTutorial1()
{
    //add a plane
    physx::PxTransform pose = 
        physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f), 
                            physx::PxQuat(physx::PxHalfPi * 1.0f, 
                            physx::PxVec3(0.0f, 0.0f, 1.0f)));
    
    
    physx::PxRigidStatic* plane = 
        physx::PxCreateStatic(*g_Physics, 
                                pose, 
                                physx::PxPlaneGeometry(), 
                                *g_PhysicsMaterial);
    
    //add it to the physX scene
    g_PhysicsScene->addActor(*plane);
   
}


void Physics::setupPhysx()
{
    physx::PxAllocatorCallback *myCallback = new MyAllocator();

    g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
    g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
    PxInitExtensions(*g_Physics);
    //create physics material  
    g_PhysicsMaterial = g_Physics->createMaterial(0.9f, 0.9f,.1f);
    physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0, -10.0f, 0);
    sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
    g_PhysicsScene = g_Physics->createScene(sceneDesc);


}

void Physics::shutdown()
{

    g_PhysicsScene->release();
    g_Physics->release();
    g_PhysicsFoundation->release();


    Gizmos::destroy();
    Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    int screen_width, screen_height;
    glfwGetWindowSize(m_window, &screen_width, &screen_height);
    if (screen_width != screen_size.x || screen_height != screen_size.y)
    {
        glViewport(0, 0, screen_width, screen_height);
        m_camera.proj = 
            glm::perspective(glm::radians(60.0f), screen_size.x / screen_size.y, 0.1f, 1000.0f);
    }


    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

	float phys_dt = 1 / 60.0f;

	GLFWwindow* curr_window = glfwGetCurrentContext();
	m_fSpawnTimer += dt;
	if ((glfwGetKey(curr_window, GLFW_KEY_SPACE) == GLFW_PRESS) && (m_fSpawnTimer > cg_fSPAWN_INTERVAL))
	{
		//	spawn a new ragdoll
		PxArticulation*	newArticulation;
		newArticulation = Ragdoll::makeRagdoll(g_Physics, ragdollData,
			PxTransform(PxVec3((float)(rand() % 1000) / 300.0f, 50, (float)(rand() % 1000) / 300.0f)),
			0.1f, g_PhysicsMaterial);
		vRagdollArticulations.push_back(newArticulation);
		g_PhysicsScene->addArticulation(*newArticulation);
		m_fSpawnTimer = 0.0f;
	}


    g_PhysicsScene->simulate(phys_dt);
    while (g_PhysicsScene->fetchResults() == false)
    {

    }


    m_camera.update(dt);
    
    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec4	vRagdollColour(1);
	PxArticulation*	pCurrentArticulation;
	for (int iCurrentArticulation = 0; iCurrentArticulation < vRagdollArticulations.size(); ++iCurrentArticulation)
	{
		pCurrentArticulation = vRagdollArticulations[iCurrentArticulation];
		PxU32	numLinks = pCurrentArticulation->getNbLinks();
		PxArticulationLink**	links = new PxArticulationLink*[numLinks];
		pCurrentArticulation->getLinks(links, numLinks);
		while (numLinks--)
		{
			PxArticulationLink*	link = links[numLinks];
			PxU32	numShapes = link->getNbShapes();
			PxShape**	shapes = new PxShape*[numShapes];
			link->getShapes(shapes, numShapes);
			while (numShapes--)
			{
				//	normally this would check for different shapes, but we are specifically only using capsules here, soooo ...
				PxCapsuleGeometry	capsule;
				shapes[numShapes]->getCapsuleGeometry(capsule);
				glm::vec3	capsulePosition(1);
				capsulePosition.x = link->getGlobalPose().p.x;
				capsulePosition.y = link->getGlobalPose().p.y;
				capsulePosition.z = link->getGlobalPose().p.z;
				PxQuat q = link->getGlobalPose().q;

				glm::quat q_glm = glm::quat(q.w, q.x, q.y, q.z);
				glm::mat4	rotation = glm::mat4(q_glm);
				
				Gizmos::addCapsule(
					capsulePosition,
					capsule.halfHeight * 2.0f, capsule.radius,
					5, 5, vRagdollColour, &rotation
					);
			}
		}
	}

    Gizmos::draw(m_camera.proj, m_camera.view);
    Gizmos::draw2D(glm::ortho(-1280/2.0f,1280.0f/2.0f,-720.0f/2.0f,720.0f/2.0f));
	
	glfwSwapBuffers(m_window);
    glfwPollEvents();
}