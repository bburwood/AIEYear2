#pragma once

#include "BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;
class World;
class BaseNPC;

const int cg_iNUM_NPCs = 100;

class UtilitySystemApplication : public BaseApplication {
public:

	UtilitySystemApplication();
	virtual ~UtilitySystemApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;

	//float		m_fTimer;
	World*		m_pWorld;
//	BaseNPC*	m_pNPC;
	BaseNPC*	m_aNPCs[cg_iNUM_NPCs];
};