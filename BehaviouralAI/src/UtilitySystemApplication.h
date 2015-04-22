#pragma once

#include "BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;
class World;
class BaseNPC;

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


	World*		m_pWorld;
	BaseNPC*	m_pNPC;
};