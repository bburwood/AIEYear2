#ifndef _BBPHYSICS_OBJECT_H_
#define _BBPHYSICS_OBJECT_H_

#include "GLMHeader.h"

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2,
};

class BBPhysicsObject
{
public:
	BBPhysicsObject();
	~BBPhysicsObject();

	virtual	void	update(glm::vec2 gravity, float timeStep) = 0;
	virtual	void	debug() = 0;
	virtual	void	MakeGizmo() = 0;
	virtual	void	resetPosition(){};


	ShapeType	m_eShapeID;

private:

};

class BBRigidBody : public BBPhysicsObject
{
public:
	BBRigidBody();
	~BBRigidBody();

	BBRigidBody(glm::vec2 a_vPosition, glm::vec2 a_vVelocity, float a_fRotation, float a_fMass);
	virtual	void	update(glm::vec2 a_fGravity, float fTimeStep);
	virtual void	debug();
	void	ApplyForce(glm::vec2 a_vForce);
	void	ApplyForceToActor(BBRigidBody*a_pActor2, glm::vec2 a_vForce);

	glm::vec2	m_vPosition;
	glm::vec2	m_vVelocity;
	glm::vec2	m_vForce;
	float	m_fMass;
	float	m_fRotation2D;	//	2D, so we only need a single float to represent our rotation

private:
	float	m_fOneOnMass;	//	1/m ... change this whenever the mass changes
};

class BBSphere : public BBRigidBody
{
public:
	BBSphere();
	~BBSphere();

	BBSphere(glm::vec2 a_vPosition, glm::vec2 a_vVelocity, float a_fMass, float a_fRadius, glm::vec4 a_vColour);
	virtual	void	MakeGizmo();

	glm::vec4	m_vColour;
	float	m_fRadius;

private:

};
/*
SAT (Separated Axis Theorem) Collision detection in 2D
1. Get vector from 1 point to the next point.  Normalise it
2. Get the vector perpendicular to it [x,y] --> [y,-x]
3. 
See here: http://www.metanetsoftware.com/technique/tutorialA.html

For 3D see the GJK theorem:
http://vec3.ca/gjk/implementation/
Video about it: http://mollyrocket.com/849
*/

#endif	//	_BBPHYSICS_OBJECT_H_