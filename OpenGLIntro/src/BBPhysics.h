#ifndef _BBPHYSICS_H_
#define _BBPHYSICS_H_

#include "BBPhysicsObject.h"
#include "GLMHeader.h"

#include <vector>

class BBPhysicsScene
{
public:
	BBPhysicsScene();
	~BBPhysicsScene();

	void	AddActor(BBPhysicsObject* a_pActor);
	void	RemoveActor(BBPhysicsObject* a_pActor);
	void	update(float dT);
	void	DebugScene();
	void	AddGizmos();

	glm::vec2	m_vGravity;
	float		m_fTimeStep;
	float		m_fMaxTimeStep;
	std::vector<BBPhysicsObject*>	m_aActors;

private:

};

class BBPhysics
{
public:
	BBPhysics();
	~BBPhysics();

	BBPhysicsScene	TheScene;

private:

};
#endif	//	_BBPHYSICS_H_