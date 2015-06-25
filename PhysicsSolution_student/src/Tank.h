#ifndef _TANK_H_
#define _TANK_H_

#include "FBXFile.h"
#include "Vertex.h"
#include "Camera.h"

#include "GLMHeader.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

class Tank
{
public:
	Tank();
	~Tank();
	void update(float fDt);
	void draw();
	void	Init(FlyCamera* a_pCamera, PxScene* a_pPhysicsScene, PxPhysics* a_pPhysics);
	void	GenerateGLMeshes(FBXFile* fbx);
	void	EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer);
	void	UpdateBones(FBXSkeleton* skeleton);

	FBXFile*	m_file;
	std::vector<OpenGLData>	m_meshes;
	unsigned int	m_uiProgramID;

private:
	mat4	m_WorldTransform;
	PxRigidDynamic*	m_pDynamicActor;
	FlyCamera*	m_FlyCamera;
	PxScene*		m_pPhysicsScene;
	PxPhysics* m_pPhysics;
	float	m_timer;
	float	m_fDensity;
	bool	m_bDrawCollisionBoxes;
};

#endif	//	_TANK_H_
