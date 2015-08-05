/*
Things to do:
* Read PhysX docs for Player controller.  Done
* Collision Shape Hierarchies - Tutorial.  This is the one that sets up the tank.
* Work through Player Controller tutorial.  Mostly done **** haven't added an animated FBX player to use.
* Work through Mesh Collision tutorial
* Work through Soft Bodies tutorial.		Done.
* Work on assessment ...

Bullet Physics is a good open source alternative to PhysX - especially if you want to publish your game without paying royalties.

*/

#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Utility.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>

#include "glm/gtc/quaternion.hpp"

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
	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.r, m_BackgroundColour.g, m_BackgroundColour.b, m_BackgroundColour.a);
    glEnable(GL_DEPTH_TEST);
	
//	void Gizmos::create(unsigned int a_maxLines /* = 0xffff */, unsigned int a_maxTris /* = 0xffff */,
//	unsigned int a_max2DLines /* = 0xff */, unsigned int a_max2DTris /* = 0xff */)
	Gizmos::create(0xA0000, 0xA0000, 0xffff, 0xffff);
//	m_bDrawGizmos = true;
//	m_bDrawGrid = true;

	m_FlyCamera = FlyCamera(vec3(-1, 10, 0.00001f), vec3(-1, 0.000001f, 0.000001f), glm::radians(50.0f), (float)BUFFER_WIDTH / (float)BUFFER_HEIGHT, 0.1f, 3000.0f);
	m_FlyCamera.SetSpeed(25.0f);
	m_FlyCamera.SetLookAt(vec3(0, 20, 40), vec3(0.0f, 10.0f, 0.0f), vec3(0, 1, 0));
	m_bFluidDynamics = true;
	m_bBoxesAndSpheres = true;
	m_bRagdolls = true;

	m_fTimer = 0.0f;
	m_bSpheresCreated = false;
	m_iNextSphereToFire = 0;
	m_fFiringTimer = 0.0f;
	m_fFiringInterval = c_fSphereFiringInterval;
	m_fFiringSpeed = c_fSphereFiringSpeed;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(BUFFER_WIDTH, BUFFER_HEIGHT);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);
	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("GeneralStuff");	//	must be a single word (no spaces) if you want to be able to resize it
	TwDefine(" GeneralStuff size='280 300' "); // resize bar
	TwAddSeparator(m_bar, "Misc Data", "");
	//TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_vLightDir, "label='Light Direction'");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Camera Speed", TW_TYPE_FLOAT, &m_FlyCamera.m_fSpeed, "min=1 max=250 step=1");
//	TwAddVarRW(m_bar, "Gravity", TW_TYPE_DIR3F, &m_vPhysXGravity, "");
	TwAddVarRW(m_bar, "Current Cloth Texture", TW_TYPE_UINT32, &m_uiCurrentTexture, "min=0 max=5 step=1");
	TwAddVarRO(m_bar, "Rendering FPS", TW_TYPE_FLOAT, &m_fFPS, "");
	
	setupPhysx();
	if (m_bBoxesAndSpheres)
	{
		SetupBoxesAndSpheres();
	}
	if (m_bFluidDynamics)
	{
		SetupFluidDynamics();
	}
	SetupCloth();
	SetupPlayerController();
	SetupRagdolls();
	SetupTriggers();
	setupVisualDebugger();

	//m_oTank.Init(&m_camera, m_pPhysicsScene);	//	initialise the tank!

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

void	Physics::SetupFluidDynamics()
{
	PxTransform	pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic*	plane = PxCreateStatic(*m_pPhysics, pose, PxPlaneGeometry(), *m_pPhysicsMaterial);
	const	PxU32	numShapes = plane->getNbShapes();
	m_pPhysicsScene->addActor(*plane);

	PxBoxGeometry	side1(4.5f, 1.0f, 0.5f);
	PxBoxGeometry	side2(0.5f, 1.0f, 4.5f);

	pose = PxTransform(PxVec3(0.0f, 0.5f, 4.0f));
	PxRigidStatic*	box = PxCreateStatic(*m_pPhysics, pose, side1, *m_pPhysicsMaterial);
	m_pPhysicsScene->addActor(*box);
	m_aFluidActors.push_back(box);

	pose = PxTransform(PxVec3(0.0f, 0.5f, -4.0f));
	box = PxCreateStatic(*m_pPhysics, pose, side1, *m_pPhysicsMaterial);
	m_pPhysicsScene->addActor(*box);
	m_aFluidActors.push_back(box);

	pose = PxTransform(PxVec3(4.0f, 0.5f, 0.0f));
	box = PxCreateStatic(*m_pPhysics, pose, side2, *m_pPhysicsMaterial);
	m_pPhysicsScene->addActor(*box);
	m_aFluidActors.push_back(box);

	pose = PxTransform(PxVec3(-4.0f, 0.5f, 0.0f));
	box = PxCreateStatic(*m_pPhysics, pose, side2, *m_pPhysicsMaterial);
	m_pPhysicsScene->addActor(*box);
	m_aFluidActors.push_back(box);

	//	now set up the particle/fluid system
	//	create the particle system in PhysX SDK
	//	set immutable properties
	PxU32	maxParticles = 6000;
	bool	perParticleRestOffset = false;
	m_pFluidSystem = m_pPhysics->createParticleFluid(maxParticles, perParticleRestOffset);
	m_pFluidSystem->setRestParticleDistance(0.35f);
	m_pFluidSystem->setDynamicFriction(0.1f);
	m_pFluidSystem->setStaticFriction(0.1f);
	m_pFluidSystem->setDamping(0.1f);
	m_pFluidSystem->setParticleMass(0.1f);
	m_pFluidSystem->setRestitution(0.0f);
	m_pFluidSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	m_pFluidSystem->setStiffness(100.0f);

	if (m_pFluidSystem)
	{
		m_pPhysicsScene->addActor(*m_pFluidSystem);
		m_pFluidEmitter = new ParticleFluidEmitter(maxParticles, PxVec3(0, 10, 0), m_pFluidSystem, 0.01f);
		m_pFluidEmitter->setStartVelocityRange(-1.0f, -50.0f, -1.0f, 1.0f, -50.0f, 1.0f);
	}
}

void	Physics::SetupPlayerController()
{
	//	now the controller stuff ...
	m_pMyHitReport = new MyControllerHitReport();
	m_pCharacterManager = PxCreateControllerManager(*m_pPhysicsScene);
	//	describe our controller
	m_ControllerDescription.height = 3.0f;
	m_ControllerDescription.radius = 0.75f;
	m_ControllerDescription.position.set(0, 1, 6);
	m_ControllerDescription.material = m_pPhysicsMaterial;
	m_ControllerDescription.reportCallback = m_pMyHitReport;	//	connect it to our collision detection routine
	m_ControllerDescription.density = 20.0f;
	m_ControllerDescription.stepOffset = 0.15f;
	m_ControllerDescription.contactOffset = 0.005f;	//	mess with the skin thickness! ... hmm, doesn't seem to do a real lot.

	//cout << "Skin thickness: " << m_ControllerDescription.contactOffset << "\n";

	//	create the player controller
	m_pPlayerController = m_pCharacterManager->createController(m_ControllerDescription);
	m_pPlayerController->setPosition(m_ControllerDescription.position);
	m_pPlayerController->getActor()->setName("MainPlayer!!");
	SetupFiltering(m_pPlayerController->getActor(), FilterGroup::ePLAYER, FilterGroup::eBOX | FilterGroup::eCLOTH | FilterGroup::eGROUND | FilterGroup::eRAGDOLL | FilterGroup::eSPHERE);

	//	set up some variables to control our player with
	m_fCharacterYVelocity = 0.0f;	//	initialise character velocity
	m_fCharacterRotation = 0.0f;	//	and rotation
	m_fPlayerGravity = -5.0f;	//	set up the player gravity
	m_pMyHitReport->clearPlayerContactNormal();	//	initialise the contact normal (what we are in contact with)
	m_aCharacterActors.push_back(m_pPlayerController->getActor());
	m_fMovementSpeed = 8.0f;	//	forward and backward movement speed
	m_fRotationSpeed = 5.0f;	//	turning speed (in radians? per second)
	m_bOnGround = false;
}

void	Physics::SetupCloth()
{
	//	set cloth properties
	float fSpringSize = 0.2f;
	uiSpringRows = 40;
	uiSpringCols = 40;
	//	this position will represent the top middle vertex
	m_aClothPos = glm::vec3(0.0f, 4.0f, 0.0f);
	//	shifting grid position for looks
	float	fHalfWidth = fSpringSize * uiSpringCols * 0.5f;

	//	generate vertices for a grid with texture coordinates
	m_uiClothVertexCount = uiSpringRows * uiSpringCols;
	m_aClothPositions = new glm::vec3[m_uiClothVertexCount];
	glm::vec2*	clothTextureCoords = new glm::vec2[m_uiClothVertexCount];
	int	iCurrentOffset = 0;
	for (int r = 0; r < uiSpringRows; ++r)
	{
		for (int c = 0; c < uiSpringCols; ++c)
		{
			m_aClothPositions[iCurrentOffset].x = m_aClothPos.x + fSpringSize * c;
			m_aClothPositions[iCurrentOffset].y = m_aClothPos.y;	//	local space - this is added to the global pose position
			m_aClothPositions[iCurrentOffset].z = m_aClothPos.z + fSpringSize * r - fHalfWidth;

			clothTextureCoords[iCurrentOffset].x = 1.0f - r / (uiSpringRows - 1.0f);
			clothTextureCoords[iCurrentOffset].y = 1.0f - c / (uiSpringCols - 1.0f);

			++iCurrentOffset;
		}
	}

	//	set up indices for the grid
	m_uiClothIndexCount = (uiSpringRows - 1) * (uiSpringCols - 1) * 2 * 3;
	unsigned int*	indices = new unsigned int[m_uiClothIndexCount];
	unsigned int*	index = indices;

	for (int r = 0; r < (uiSpringRows - 1); ++r)
	{
		for (int c = 0; c < (uiSpringCols - 1); ++c)
		{
			//	indices for the 4 quad corner vertices
			unsigned int	i0 = r * uiSpringCols + c;
			unsigned int	i1 = i0 + 1;
			unsigned int	i2 = i0 + uiSpringCols;
			unsigned int	i3 = i2 + 1;

			//	every second quad changes the triangle order
			if ((c + r) % 2)
			{
				*index++ = i0;	*index++ = i2;	*index++ = i1;
				*index++ = i1;	*index++ = i2;	*index++ = i3;
			}
			else
			{
				*index++ = i0;	*index++ = i2;	*index++ = i3;
				*index++ = i0;	*index++ = i3;	*index++ = i1;
			}
		}
	}
	CreateCloth(m_aClothPos, m_uiClothVertexCount, m_uiClothIndexCount, m_aClothPositions, indices);
	//	now we need to set up the opengl stuff for it ...
	glGenBuffers(1, &m_uiClothVBO);
	glGenBuffers(1, &m_uiClothIBO);
	glGenVertexArrays(1, &m_uiClothVAO);
	glBindVertexArray(m_uiClothVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiClothVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiClothIBO);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) + sizeof(glm::vec2)) * m_uiClothVertexCount, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_uiClothVertexCount, m_aClothPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_uiClothVertexCount, sizeof(glm::vec2) * m_uiClothVertexCount, clothTextureCoords);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_uiClothIndexCount, indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(glm::vec3) * m_uiClothVertexCount));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	ReloadShader();
	//	load the textures, and get their ID's in an array
	LoadTexture("textures/gold-metal-texture-tracery.jpg", m_uiClothTexture[0]);
	cout << "Cloth TextureID: " << m_uiClothTexture[0] << "\n";
	LoadTexture("textures/molten_gold_texture__tileable___2048x2048__by_fabooguy-d705j3c.jpg", m_uiClothTexture[1]);
	cout << "Cloth TextureID: " << m_uiClothTexture[1] << "\n";
	LoadTexture("textures/gold_background_texture_69796_2048x2048.jpg", m_uiClothTexture[2]);
	cout << "Cloth TextureID: " << m_uiClothTexture[2] << "\n";
	LoadTexture("textures/Metal_02_UV_H_CM_1.jpg", m_uiClothTexture[3]);
	cout << "Cloth TextureID: " << m_uiClothTexture[3] << "\n";
	LoadTexture("textures/smooth_metal_plate_texture_seamless_tileable_by_hhh316-d782dao.jpg", m_uiClothTexture[4]);
	cout << "Cloth TextureID: " << m_uiClothTexture[4] << "\n";
	LoadTexture("textures/Beveled_Checker_Board_Seamless_by_FantasyStock_2048.jpg", m_uiClothTexture[5]);
	cout << "Cloth TextureID: " << m_uiClothTexture[5] << "\n";
	m_uiCurrentTexture = 0;
	delete[] indices;
	delete[] clothTextureCoords;
}

void	Physics::CreateCloth(glm::vec3 &a_vPosition, unsigned int &a_uiVertexCount, unsigned int &a_uiIndexCount, const glm::vec3* a_vVertices, unsigned int *a_uiIndices)
{
	PxClothMeshDesc	clothDesc;
	clothDesc.points.count = a_uiVertexCount;
	clothDesc.invMasses.count = a_uiVertexCount;
	clothDesc.triangles.count = a_uiIndexCount / 3;

	PxClothParticle*	particles = new PxClothParticle[a_uiVertexCount];
	for (int i = 0; i < a_uiVertexCount; ++i)
	{
		particles[i].pos.x = a_vVertices[i].x;
		particles[i].pos.y = a_vVertices[i].y;
		particles[i].pos.z = a_vVertices[i].z;
		particles[i].invWeight = 1;	//	set to zero for static points to hang from
	}
	particles[0].invWeight = 0;	//	set to zero for static points to hang from
	particles[uiSpringCols - 1].invWeight = 0;	//	set to zero for static points to hang from

	clothDesc.points.data = particles;
	clothDesc.points.stride = sizeof(PxClothParticle);

	clothDesc.invMasses.data = &particles[0].invWeight;
	clothDesc.invMasses.stride = sizeof(PxClothParticle);

	clothDesc.triangles.data = a_uiIndices;
	clothDesc.triangles.stride = sizeof(unsigned int) * 3;

	PxClothFabric*	fabric = PxClothFabricCreate(*m_pPhysics, clothDesc, PxVec3(0, -1, 0));
	PxVec3	position;
	position.x = a_vPosition.x;
	position.y = a_vPosition.y;
	position.z = a_vPosition.z;

	PxTransform	clothTransform = PxTransform(position);
	m_pCloth = m_pPhysics->createCloth(clothTransform, *fabric, particles, PxClothFlag::eSCENE_COLLISION);
	m_pCloth->setSolverFrequency(240.0f);

	m_pPhysicsScene->addActor(*m_pCloth);

	delete[] particles;
}

void Physics::SetupBoxesAndSpheres()
{
	//	add a plane to the scene
	PxTransform	transform = PxTransform(PxVec3(0.0f, -1.0f, 0.0f), PxQuat((float)PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f).getNormalized()) * PxQuat((float)PxHalfPi * 0.20f, PxVec3(0.0f, -1.0f, 0.0f).getNormalized()));
	
	PxRigidStatic*	plane = PxCreateStatic(*m_pPhysics, transform, PxPlaneGeometry(), *m_pPhysicsMaterial);
	//transform = plane->getGlobalPose()
	
	//transform(PxVec3(0.0f, -5.0f, 0.0f));
	
	//PxTransform(PxVec3(0.0f, -5.0f, 0.0f))
	m_pPhysicsScene->addActor(*plane);

	//	add a box to the scene
	float	density = 5.0f;
	PxBoxGeometry	box(1.0f, 1.0f, 1.0f);
	PxTransform		boxTransform(PxVec3(-3.0f, 5.0f, 3.0f));
	m_pBoxActor = PxCreateDynamic(*m_pPhysics, boxTransform, box, *m_pPhysicsMaterial, density);
	m_pBoxActor->setName("BigBox!!");
//	m_pBoxActor->userData = &(this->SpawnRagdoll);
	SetupFiltering(m_pBoxActor, FilterGroup::eBOX, FilterGroup::ePLAYER | FilterGroup::eCLOTH | FilterGroup::eGROUND | FilterGroup::eRAGDOLL | FilterGroup::eSPHERE);
//	SetShapeAsTrigger(m_pBoxActor);
	m_pPhysicsScene->addActor(*m_pBoxActor);

	box.halfExtents.x = 4.0f;
	box.halfExtents.y = 4.0f;
	box.halfExtents.z = 4.0f;
	boxTransform = PxTransform(PxVec3(10.0f, 3.0f, 10.0f));
	m_pTriggerBoxActor = PxCreateStatic(*m_pPhysics, boxTransform, box, *m_pPhysicsMaterial);
	m_pTriggerBoxActor->setName("TriggerBox!!");
	//	m_pBoxActor->userData = &(this->SpawnRagdoll);
	SetupFiltering(m_pTriggerBoxActor, FilterGroup::eBOX, FilterGroup::ePLAYER | FilterGroup::eCLOTH | FilterGroup::eGROUND | FilterGroup::eRAGDOLL | FilterGroup::eSPHERE);
	SetShapeAsTrigger(m_pTriggerBoxActor);
	m_pPhysicsScene->addActor(*m_pTriggerBoxActor);



	//	now create lots of boxes!!! ...
	for (int i = 0; i < c_iNumBoxes; ++i)
	{
		PxBoxGeometry	tBox(c_fBoxSize, c_fBoxSize, c_fBoxSize);
		PxTransform		tBoxTransform(PxVec3((float)(rand() % 40) - 20.0f, 50.0f + c_fBoxSize + (float)i * 0.2f, (float)(rand() % 40) - 80.0f));
		m_aBoxes[i] = PxCreateDynamic(*m_pPhysics, tBoxTransform, tBox, *m_pPhysicsMaterial, density);
		m_aBoxes[i]->setName("SmallBox!");
		SetupFiltering(m_aBoxes[i], FilterGroup::eBOX, FilterGroup::ePLAYER | FilterGroup::eCLOTH | FilterGroup::eGROUND | FilterGroup::eRAGDOLL | FilterGroup::eSPHERE);
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
		PxTransform		tSphereTransform(PxVec3((float)(rand() % 20) - 10.0f, 40.0f + c_fSphereRadius + (float)i * 0.2f, (float)(rand() % 20) - 50.0f));
		m_aSpheres[i] = PxCreateDynamic(*m_pPhysics, tSphereTransform, tSphere, *m_pPhysicsMaterial, density);
		SetupFiltering(m_aSpheres[i], FilterGroup::eSPHERE, FilterGroup::ePLAYER | FilterGroup::eCLOTH | FilterGroup::eGROUND | FilterGroup::eRAGDOLL | FilterGroup::eBOX);
		m_aSpheres[i]->setName("Sphere!");
		m_pPhysicsScene->addActor(*m_aSpheres[i]);
		//	now make their colours
		m_aSphereColours[i] = vec4((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, 1.0f);
	}
	m_bSpheresCreated = true;
}

void	Physics::SetupRagdolls()
{
	m_iNextRagdollToSpawn = 0;
	m_iRagdollsSpawned = 0;
	m_fRagdollSpawnTimer = c_fRagdollSpawnInterval;
	for (int i = 0; i < c_iNumRagdolls; ++i)
	{
		//	initialise the array elements to nullptr
		m_aRagdollArticulations[i] = nullptr;
	}
}

void	Physics::SpawnRagdoll()
{
	//	spawn a new ragdoll
	m_aRagdollArticulations[m_iNextRagdollToSpawn] = Ragdoll::makeRagdoll(m_pPhysics, ragdollData,
		PxTransform(PxVec3((float)(rand() % 1000) / 50.0f - 10.0f, 20.0f, (float)(rand() % 1000) / 50.0f - 10.0f)),
		0.1f, m_pPhysicsMaterial);
	m_pPhysicsScene->addArticulation(*(m_aRagdollArticulations[m_iNextRagdollToSpawn]));
}

void	Physics::SetupTriggers()
{
	m_pMyCollisionCallback = new MyCollisionCallback();
	m_pPhysicsScene->setSimulationEventCallback(m_pMyCollisionCallback);
}

void Physics::setupPhysx()
{
	m_DefaultFilterShader = PxDefaultSimulationFilterShader;
	m_MyFilterShader = MyFilterShader;
	PxAllocatorCallback*	pMyCallback = new AIEAllocator();

	m_pPhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *pMyCallback, m_DefaultErrorCallback);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*m_pPhysics);

	PxSceneDesc	oSceneDesc(m_pPhysics->getTolerancesScale());
	oSceneDesc.gravity = PxVec3(0.0f, -27.0f, 0.0f);
	//oSceneDesc.filterShader = &PxDefaultSimulationFilterShader;	//	old version
	oSceneDesc.filterShader = MyFilterShader;
	oSceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(8);	//	only use 1 thread - can change this
	m_pPhysicsScene = m_pPhysics->createScene(oSceneDesc);

	m_pPhysicsMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
}

void Physics::shutdown()
{
	for (int i = 0; i < c_iNumRagdolls; ++i)
	{
		//	clean up the ragdoll articulation array
		if (m_aRagdollArticulations[i] != nullptr)
		{
			m_aRagdollArticulations[i]->release();
			m_aRagdollArticulations[i] = nullptr;
		}
	}

	//	now clean up the PhysX system
	if (m_aClothPositions != nullptr)
	{
		delete[] m_aClothPositions;
	}
	delete m_pMyHitReport;
	delete m_pFluidEmitter;

	m_pPhysicsScene->release();
	m_pPhysics->release();
	m_pPhysicsFoundation->release();

    Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
	Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

    Gizmos::clear();

    float dt = (float)glfwGetTime();
	m_fFPS = 1.0f / dt;	//	this needs to be *before* adjusting dt if needed
	dt = dt > 0.025f ? 0.025f : dt;	//	limit the maximum time step - make sure dt is a reasonable interval of no more than 1/40 th of a second
    glfwSetTime(0.0);
	m_fTimer += dt;
	if (m_bBoxesAndSpheres)
	{
		if ((m_fTimer > 5.0f) && !m_bSpheresCreated)
		{
			CreateSpheres();
		}
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

	if (m_bBoxesAndSpheres)
	{
		if (m_bSpheresCreated)
		{
			m_fFiringTimer += dt;
			if ((glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) && (m_fFiringTimer > m_fFiringInterval))
			{
				//	fire the next sphere in the array in the forward direction of the camera, from just below the camera
				vec3	vCamPos = m_FlyCamera.m_worldTransform[3].xyz;
				vec3	vForward = (m_FlyCamera.m_worldTransform[2].xyz) * (-1.0f);
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
	}

	if (m_bFluidDynamics)
	{
		//	player movement in fluid dynamics sim
		UpdatePlayerController(dt);
	}

	if (m_bRagdolls)
	{
		m_fRagdollSpawnTimer += dt;
		if ((m_fRagdollSpawnTimer > c_fRagdollSpawnInterval) && (glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS))
		{
			//	hit 'F' to drop a ragdoll in the pool
			if (m_iRagdollsSpawned < c_iNumRagdolls)
			{
				//	spawn a new ragdoll
				SpawnRagdoll();
				++m_iRagdollsSpawned;
				m_iNextRagdollToSpawn = m_iRagdollsSpawned % c_iNumRagdolls;
				m_fRagdollSpawnTimer = 0.0f;
			}
			else
			{
				//	we have already filled our ragdoll array, so reuse an existing one
				m_aRagdollArticulations[m_iNextRagdollToSpawn]->release();
				SpawnRagdoll();
				m_iNextRagdollToSpawn = (m_iNextRagdollToSpawn + 1) % c_iNumRagdolls;
				m_fRagdollSpawnTimer = 0.0f;
			}
		}
	}

	m_pPhysicsScene->simulate(dt);

	while (m_pPhysicsScene->fetchResults() == false)
	{

	}

	//m_oTank.update(dt);

	//	get the box transform
	PxTransform	boxTransform;
	//	get its position
	vec3	vBoxPosition;
	//	get its rotation
	glm::quat	qBoxRotation;
	//	add it as a gizmo
	glm::mat4	rot;
	if (m_bBoxesAndSpheres)
	{
		//	get the box transform
		boxTransform = m_pBoxActor->getGlobalPose();
		//	get its position
		vBoxPosition.x = boxTransform.p.x;
		vBoxPosition.y = boxTransform.p.y;
		vBoxPosition.z = boxTransform.p.z;
		//	get its rotation
		qBoxRotation.x = boxTransform.q.x;
		qBoxRotation.y = boxTransform.q.y;
		qBoxRotation.z = boxTransform.q.z;
		qBoxRotation.w = boxTransform.q.w;
		//	add it as a gizmo
		glm::mat4	rot(qBoxRotation);
		Gizmos::addAABBFilled(vBoxPosition, vec3(1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), &rot);

		//	get the box transform
		boxTransform = m_pTriggerBoxActor->getGlobalPose();
		//	get its position
		vBoxPosition.x = boxTransform.p.x;
		vBoxPosition.y = boxTransform.p.y;
		vBoxPosition.z = boxTransform.p.z;
		//	get its rotation
		qBoxRotation.x = boxTransform.q.x;
		qBoxRotation.y = boxTransform.q.y;
		qBoxRotation.z = boxTransform.q.z;
		qBoxRotation.w = boxTransform.q.w;
		//	add it as a gizmo
		rot = glm::mat4(qBoxRotation);
		Gizmos::addAABB(vBoxPosition, vec3(4.0f, 4.0f, 4.0f), vec4(0.5f, 0.5f, 1.0f, 1.0f), &rot);

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
	}
	glm::vec3	vExtents(0.0f);
	if (m_bFluidDynamics)
	{
		/*
		PxBoxGeometry	side1(4.5f, 1.0f, 0.5f);
		PxBoxGeometry	side2(0.5f, 1.0f, 4.5f);
		pose = PxTransform(PxVec3(0.0f, 0.5f, 4.0f));
		pose = PxTransform(PxVec3(0.0f, 0.5f, -4.0f));
		pose = PxTransform(PxVec3(4.0f, 0.5f, 0.0f));
		pose = PxTransform(PxVec3(-4.0f, 0.5f, 0.0f));
		*/
		vExtents.x = 4.5f;
		vExtents.y = 1.0f;
		vExtents.z = 0.5f;
		vBoxPosition.x = 0.0f;
		vBoxPosition.y = 0.5f;
		vBoxPosition.z = 4.0f;
		Gizmos::addAABBFilled(vBoxPosition, vExtents, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vBoxPosition.z = -4.0f;
		Gizmos::addAABBFilled(vBoxPosition, vExtents, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vBoxPosition.x = 4.0f;
		vBoxPosition.z = 0.0f;
		vExtents.x = 0.5f;
		vExtents.z = 4.5f;
		Gizmos::addAABBFilled(vBoxPosition, vExtents, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vBoxPosition.x = -4.0f;
		Gizmos::addAABBFilled(vBoxPosition, vExtents, vec4(1.0f, 0.0f, 0.0f, 1.0f));

		if (m_pFluidEmitter)
		{
			m_pFluidEmitter->update(dt);
			m_pFluidEmitter->renderParticles();
		}
		//	now loop through the character actors array and draw them
		for (int i = 0; i < m_aCharacterActors.size(); ++i)
		{
			PxRigidActor*	pRigidActor = (PxRigidActor*)m_aCharacterActors[i];
			PxShape*	shapes[8] = {};
			int	iShapeCount = pRigidActor->getShapes(shapes, 8);
			for (int j = 0; j < iShapeCount; ++j)
			{
				AddWidget(shapes[j], pRigidActor);
			}
		}

		//	now render the cloth
		PxClothParticleData*	pParticles = m_pCloth->lockParticleData();
		for (int i = 0; i < m_uiClothVertexCount; ++i)
		{
			m_aClothPositions[i].x = pParticles->particles[i].pos.x;
			m_aClothPositions[i].y = pParticles->particles[i].pos.y;
			m_aClothPositions[i].z = pParticles->particles[i].pos.z;
			m_aClothPositions[i] += m_aClothPos;
		}
		pParticles->unlock();

		//	re-buffer the new positions data into the cloth VBO for rendering
		glBindBuffer(GL_ARRAY_BUFFER, m_uiClothVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_uiClothVertexCount, m_aClothPositions);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_FlyCamera.update(dt);

    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_oTank.draw();

	if (m_bFluidDynamics)
	{
		//use shader program ... and pass stuff into it if necessary
		glUseProgram(m_uiClothShader);
		unsigned int	uiProjViewUniform = glGetUniformLocation(m_uiClothShader, "projectionView");
		unsigned int	uiClothTextureUniform = glGetUniformLocation(m_uiClothShader, "diffuseMap");
		glUniformMatrix4fv(uiProjViewUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
		glUniform1i(uiClothTextureUniform, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_uiClothTexture[m_uiCurrentTexture]);
		glBindVertexArray(m_uiClothVAO);
		glDrawElements(GL_TRIANGLES, m_uiClothIndexCount, GL_UNSIGNED_INT, 0);
	}
	DrawRagdolls();

	Gizmos::draw(m_FlyCamera.m_projectionViewTransform);
	TwDraw();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void	Physics::DrawRagdolls()
{
	glm::vec4	vRagdollColour(1);
	PxArticulation*	pCurrentArticulation;
	for (int iCurrentArticulation = 0; iCurrentArticulation < c_iNumRagdolls; ++iCurrentArticulation)
	{
		if (m_aRagdollArticulations[iCurrentArticulation] != nullptr)
		{
			pCurrentArticulation = m_aRagdollArticulations[iCurrentArticulation];
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
					PxVec3	pos = link->getGlobalPose().p;
					capsulePosition.x = pos.x;
					capsulePosition.y = pos.y;
					capsulePosition.z = pos.z;
					PxQuat q = link->getGlobalPose().q;

					//glm::quat q_glm = glm::quat(q.w, q.x, q.y, q.z);
					//glm::mat4	rotation = glm::mat4(q_glm);
					glm::mat4	rotation = glm::mat4(glm::quat(q.w, q.x, q.y, q.z));

					Gizmos::addCapsule(
						capsulePosition,
						capsule.halfHeight * 2.0f, capsule.radius,
						5, 5, vRagdollColour, &rotation
						);
				}
			}
		}
	}
}

void	Physics::AddSphere(PxShape* shape, PxRigidActor* rigidActor)
{
	//	get the global position of the actor
	PxTransform	pose = rigidActor->getGlobalPose();

	//	get what the sphere actually looks like
	PxSphereGeometry	geo;
	bool	status = shape->getSphereGeometry(geo);
	float	radius = 0.0;

	//	make sure nothing broke
	if (status)
	{
		radius = geo.radius;
	}
	//	pull information out of PxTransform into glm vars
	glm::vec3	position = glm::vec3(pose.p.x, pose.p.y, pose.p.z);
	glm::quat	rotation = glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z);
	glm::mat4	matRotation = glm::mat4(rotation);
	//	actually render the sphere
	Gizmos::addSphereFilled(position, radius, 5, 5, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &matRotation);
}

void	Physics::AddBox(PxShape* shape, PxRigidActor* rigidActor)
{
	//	get the global position of the actor
	PxTransform	pose = rigidActor->getGlobalPose();

	//	get what the sphere actually looks like
	PxBoxGeometry	geo;
	bool	status = shape->getBoxGeometry(geo);
	PxVec3	vExtents;

	//	make sure nothing broke
	if (status)
	{
		vExtents = geo.halfExtents;
	}
	//	pull information out of PxTransform into glm vars
	glm::vec3	position = glm::vec3(pose.p.x, pose.p.y, pose.p.z);
	glm::quat	rotation = glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z);
	glm::mat4	matRotation = glm::mat4(rotation);
	//	actually render the sphere
	Gizmos::addAABBFilled(position, glm::vec3(vExtents.x, vExtents.y, vExtents.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &matRotation);
}

void	Physics::AddCapsule(PxShape* shape, PxRigidActor* rigidActor)
{
	//	get the global position of the actor
	PxTransform	pose = rigidActor->getGlobalPose();

	//	get what the sphere actually looks like
	PxCapsuleGeometry	geo;
	bool	status = shape->getCapsuleGeometry(geo);
	float	radius = 0.0;
	float	halfHeight = 0.0;
	//	make sure nothing broke
	if (status)
	{
		halfHeight = geo.halfHeight;
		radius = geo.radius;
	}
	//	pull information out of PxTransform into glm vars
	glm::vec3	position = glm::vec3(pose.p.x, pose.p.y, pose.p.z);
	glm::quat	rotation = glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z);
	glm::mat4	matRotation = glm::mat4(rotation);
	//	actually render the capsule
	Gizmos::addCapsule(position, (halfHeight) * 2.0f, radius, 8, 8, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &matRotation);
}

void	Physics::AddWidget(PxShape* shape, PxRigidActor* rigidActor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
	{
		AddBox(shape, rigidActor);
		break;
	}
	case PxGeometryType::eSPHERE:
	{
		AddSphere(shape, rigidActor);
		break;
	}
	case PxGeometryType::eCAPSULE:
	{
		//std::cout << "drawing capsule\n";
		AddCapsule(shape, rigidActor);
		break;
	}
	default:
		break;
	}
}

void	Physics::UpdatePlayerController(float dt)
{
	//	check if we have a contact normal.  If y > 0.3 we assume this is solid ground.  This is rather primitive - try thinking of something better ...
	if (m_pMyHitReport->getPlayerContactNormal().y > 0.1f)
	{
		m_fCharacterYVelocity = -0.1f;
		m_bOnGround = true;
		//cout << "OnGround!\n";
	}
	else
	{
		m_fCharacterYVelocity += m_fPlayerGravity * dt;
		m_bOnGround = false;
	}
	m_pMyHitReport->clearPlayerContactNormal();
	const PxVec3	up(0, 1, 0);
	//	scan the keys and set up our intended velocity based on a global transform
	PxVec3	velocity(0, m_fCharacterYVelocity, 0);
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		velocity.x += m_fMovementSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		velocity.x -= m_fMovementSpeed * dt;
	}
	//	add in z movement and jumping - turn left and right
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_fCharacterRotation += m_fRotationSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_fCharacterRotation -= m_fRotationSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		//	jump when the right control key is pressed
		m_fCharacterYVelocity = 1.0f;
		velocity.y = 1.0f;
	}

	float	fMinDistance = 0.001f;
	PxControllerFilters	filter;
	//	make controls relative to player facing direction
	PxQuat	rotation(m_fCharacterRotation, PxVec3(0, 1, 0));
	//PxVec3	newVelocity(0, m_fCharacterYVelocity, 0);
	//	move the controller
	PxVec3	displacement = rotation.rotate(velocity);
	PxControllerCollisionFlags flags = m_pPlayerController->move(displacement, fMinDistance, dt, filter);

	//	now draw a direction vector for the controller
	//	get the global position of the actor
	PxExtendedVec3	position = m_pPlayerController->getPosition();
	glm::vec3	glmPosition;
	glmPosition.x = (float)position.x;
	glmPosition.y = (float)position.y;
	glmPosition.z = (float)position.z;
	Gizmos::addLine(glmPosition, glmPosition + glm::vec3(displacement.x, displacement.y, displacement.z) * 10.0f, glm::vec4(0, 0, 1, 1));
}

void	Physics::ReloadShader()
{
	//	may need to add a check for not loaded programs here, given I'm using this to do the initial shader loading as well
	glDeleteProgram(m_uiClothShader);
	LoadShader("shaders/BasicVertex.glsl", 0, "shaders/BasicFragment.glsl", &m_uiClothShader);
}
