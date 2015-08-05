#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "AntTweakBar.h"
#include "Application.h"
#include "Camera.h"
#include "MyController.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"
#include "Ragdoll.h"
#include "Triggers.h"
#include <vector>

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

const	int	c_iNumBoxes = 50;
const	int	c_iNumSpheres = 50;
const	int	c_iNumRagdolls = 10;
const	int	c_iNumTextures = 6;
const	float	c_fBoxSize = 0.5f;
const	float	c_fSphereRadius = 0.50f;
const	float	c_fSphereFiringSpeed = 10.0f;
const	float	c_fRagdollTriggerFiringSpeed = 20.0f;
const	float	c_fSphereFiringInterval = 0.10f;
const	float	c_fRagdollSpawnInterval = 0.20f;

class Physics : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();
	void	ReloadShader();

    void	setupPhysx();
    void	SetupBoxesAndSpheres();
	void	setupVisualDebugger();
	void	SetupFluidDynamics();
	void	SetupCloth();
	void	SetupPlayerController();
	void	SetupTriggers();
	//PxFilterFlags	MyFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
	void	SetupRagdolls();
	void	SpawnRagdoll();
	void	DrawRagdolls();
	void	CreateCloth(glm::vec3 &a_vPosition, unsigned int &a_uiVertexCount, unsigned int &a_uiIndexCount, const glm::vec3* a_vVertices, unsigned int *a_uiIndices);
	void	CreateSpheres();
	void	AddSphere(PxShape* shape, PxRigidActor* rigidActor);
	void	AddBox(PxShape* shape, PxRigidActor* rigidActor);
	void	AddWidget(PxShape* shape, PxRigidActor* rigidActor);
	void	AddCapsule(PxShape* shape, PxRigidActor* rigidActor);
	void	UpdatePlayerController(float fDt);

	FlyCamera m_FlyCamera;
	TwBar*	m_bar;

	//	Tank variables
	//Tank	m_oTank;

	//	PhysX variables
	PxFoundation*	m_pPhysicsFoundation;
	PxPhysics*		m_pPhysics;
	PxScene*		m_pPhysicsScene;

	PxDefaultErrorCallback	m_DefaultErrorCallback;
	PxDefaultAllocator		m_DefaultAllocator;
	PxSimulationFilterShader	m_DefaultFilterShader;
	PxSimulationFilterShader	m_MyFilterShader;

	PxMaterial*	m_pPhysicsMaterial;
	PxMaterial*	m_pBoxMaterial;
	PxCooking*	m_pPhysicsCooker;

	PxRigidStatic*	m_pTriggerBoxActor;
	PxRigidDynamic*	m_pActor;
	PxRigidDynamic*	m_pBoxActor;
	PxRigidDynamic*	m_aBoxes[c_iNumBoxes];
	PxRigidDynamic*	m_aSpheres[c_iNumSpheres];
	PxArticulation*	m_aRagdollArticulations[c_iNumRagdolls];

	MyControllerHitReport*	m_pMyHitReport;
	PxControllerManager*	m_pCharacterManager;
	PxCapsuleControllerDesc	m_ControllerDescription;
	PxController*	m_pPlayerController;
	PxSimulationEventCallback*	m_pMyCollisionCallback;
	float	m_fCharacterYVelocity;
	float	m_fCharacterRotation;
	float	m_fPlayerGravity;
	float	m_fMovementSpeed;
	float	m_fRotationSpeed;
	bool	m_bOnGround;

	//	cloth variables
	PxCloth*	m_pCloth;
	unsigned int	m_uiClothShader;
	unsigned int	m_uiClothTexture[c_iNumTextures];
	unsigned int	m_uiCurrentTexture;
	unsigned int	m_uiClothIndexCount;
	unsigned int	m_uiClothVertexCount;
	unsigned int	m_uiClothVAO;
	unsigned int	m_uiClothVBO;
	unsigned int	m_uiClothIBO;
	//	unsigned int	m_uiClothTextureVBO;
	unsigned int	uiSpringRows;
	unsigned int	uiSpringCols;
	glm::vec3*		m_aClothPositions;
	glm::vec3		m_aClothPos;

	glm::vec3		m_vPhysXGravity;

	PxParticleFluid*	m_pFluidSystem;
	ParticleFluidEmitter*	m_pFluidEmitter;
	std::vector<PxRigidStatic*>	m_aFluidActors;
	std::vector<PxRigidDynamic*>	m_aCharacterActors;

	glm::vec4	m_aColours[c_iNumBoxes];
	glm::vec4	m_aSphereColours[c_iNumSpheres];
	vec4	m_BackgroundColour;
	int		m_iNextSphereToFire;
	int		m_iNextRagdollToSpawn;
	int		m_iRagdollsSpawned;
	float	m_fRagdollSpawnTimer;
	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fFiringSpeed;
	float	m_fTimer;
	float	m_fFPS;
	bool	m_bSpheresCreated;
	bool	m_bBoxesAndSpheres;
	bool	m_bFluidDynamics;
	bool	m_bRagdolls;
};

#endif //_PHYSICS_H_