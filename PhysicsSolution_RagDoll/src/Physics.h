#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Application.h"
#include "Camera.h"

#include <vector>

#include <PxPhysicsAPI.h>
#include <PxScene.h>

using	namespace	physx;

const	float	cg_fSPAWN_INTERVAL = 0.1f;

class Physics : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();

    void setupPhysx();
    void setupTutorial1();

    FlyCamera m_camera;

    glm::vec2 screen_size;

    physx::PxFoundation* g_PhysicsFoundation;
    physx::PxPhysics* g_Physics;
    physx::PxScene* g_PhysicsScene;
    physx::PxDefaultErrorCallback gDefaultErrorCallback;
    physx::PxDefaultAllocator gDefaultAllocatorCallback;
    physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
    physx::PxMaterial* g_PhysicsMaterial;
    physx::PxMaterial* g_boxMaterial;
    physx::PxCooking* g_PhysicsCooker;

   // physx::PxRigidDynamic* dynamicActors[32];

	std::vector<PxArticulation*>	vRagdollArticulations;
	float	m_fSpawnTimer;
};

#endif //	_PHYSICS_H_
