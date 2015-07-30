#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Application.h"
#include "Camera.h"
#include "MyController.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"
#include "Tank.h"
#include <vector>

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
	void	SetupFluidDynamics();
	void	CreateSpheres();
	void	AddSphere(PxShape* shape, PxRigidActor* rigidActor);
	void	AddBox(PxShape* shape, PxRigidActor* rigidActor);
	void	AddWidget(PxShape* shape, PxRigidActor* rigidActor);
	void	AddCapsule(PxShape* shape, PxRigidActor* rigidActor);

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

	PxRigidDynamic*	m_pActor;
	PxRigidDynamic*	m_pBoxActor;
	PxRigidDynamic*	m_aBoxes[c_iNumBoxes];
	PxRigidDynamic*	m_aSpheres[c_iNumSpheres];

	MyControllerHitReport*	m_pMyHitReport;
	PxControllerManager*	m_pCharacterManager;
	PxCapsuleControllerDesc	m_ControllerDescription;
	PxController*	m_pPlayerController;
	float	m_fCharacterYVelocity;
	float	m_fCharacterRotation;
	float	m_fPlayerGravity;
	float	m_fMovementSpeed;
	float	m_fRotationSpeed;
	bool	m_bOnGround;

	PxParticleFluid*	m_pFluidSystem;
	ParticleFluidEmitter*	m_pFluidEmitter;
	std::vector<PxRigidStatic*>	m_aFluidActors;
	std::vector<PxRigidDynamic*>	m_aCharacterActors;

	glm::vec4	m_aColours[c_iNumBoxes];
	glm::vec4	m_aSphereColours[c_iNumSpheres];
	int		m_iNextSphereToFire;
	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fFiringSpeed;
	float	m_fTimer;
	bool	m_bSpheresCreated;
	bool	m_bBoxesAndSpheres;
	bool	m_bFluidDynamics;
};

#endif //_PHYSICS_H_