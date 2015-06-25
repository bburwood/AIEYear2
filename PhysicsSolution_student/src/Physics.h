#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "Tank.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

const	int	c_iNumBoxes = 75;
const	int	c_iNumSpheres = 50;
const	float	c_fBoxSize = 0.5f;
const	float	c_fSphereRadius = 0.80f;
const	float	c_fSphereFiringSpeed = 100.0f;
const	float	c_fSphereFiringInterval = 0.05f;

class Physics : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();

    void setupPhysx();
    void setupTutorial1();
	void	setupVisualDebugger();
	void	CreateSpheres();

    FlyCamera m_camera;

	//	Tank variables
	Tank	m_oTank;

	//	PhysX variables
	PxFoundation*	m_pPhysicsFoundation;
	PxPhysics*		m_pPhysics;
	PxScene*		m_pPhysicsScene;

	PxDefaultErrorCallback	m_DefaultErrorCallback;
	PxDefaultAllocator		m_DefaultAllocator;
	PxSimulationFilterShader	m_DefaultFilterShader;

	PxMaterial*	m_pPhysicsMaterial;
	PxMaterial*	m_pBoxMaterial;
	PxCooking*	m_pPhysicsCooker;

	PxRigidDynamic*	m_pBoxActor;
	PxRigidDynamic*	m_aBoxes[c_iNumBoxes];
	PxRigidDynamic*	m_aSpheres[c_iNumSpheres];
	glm::vec4	m_aColours[c_iNumBoxes];
	glm::vec4	m_aSphereColours[c_iNumSpheres];
	int		m_iNextSphereToFire;
	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fFiringSpeed;
	float	m_fTimer;
	bool	m_bSpheresCreated;
};

#endif //CAM_PROJ_H_