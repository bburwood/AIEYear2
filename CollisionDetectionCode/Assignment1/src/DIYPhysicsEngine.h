#pragma once

#include <vector>
#include <iostream>
#include <vector>
#define GLM_SWIZZLE


#include <gl_core_4_4.h>
#include <glfw3.h>
#include <aieutilities/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>



enum ShapeType
{
	PLANE = 0,
	SPHERE =1,
	BOX = 2,
	NUMBERSHAPE = 3,
};

class PhysicsObject
{
public:
	ShapeType _shapeID;
	void virtual update(glm::vec2 gravity,float timeStep) = 0;
	void virtual debug() =0;
	void virtual makeGizmo() =0;
	void virtual resetPosition(){};

};

class PlaneClass: public PhysicsObject
{
public:
	glm::vec2 normal;
	float distance;
	void virtual update(glm::vec2 gravity,float timeStep){};
	void virtual debug(){};
	void virtual makeGizmo();
	PlaneClass(glm::vec2 normal,float distance);
	PlaneClass();
};

class DIYRigidBody: public PhysicsObject
{
public:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 colour;
	glm::vec2 oldPosition;
	glm::vec2 contactPoint; //if a collision occurs then this is the point of contact
	bool	bIsStatic;
	float mass;
	float rotation2D; //2D so we only need a single float to represent our rotation about Z
	float	staticFriction;
	float	dynamicFriction;
	float	angularVelocity;
	float	momentOfInertia;
	glm::vec2	totalForce;
	float	totalTorque;
	glm::mat4 rotationMatrix;
	DIYRigidBody(glm::vec2 position,glm::vec2 velocity,float rotation,float mass);
	virtual void update(glm::vec2 gravity,float timeStep);
	virtual void debug();
	virtual void collisionResponse(glm::vec2 collisionPoint);
	virtual void resetPosition(){position = oldPosition;};
	void applyForce(glm::vec2 force);
	void	applyForceAtPoint(glm::vec2 force, glm::vec2 point);
	void applyForceToActor(DIYRigidBody* actor2, glm::vec2 force);

};

class Joint
{
public:
	Joint();
	~Joint();

	virtual	void	Update(float fDt) = 0;
	virtual	void	DrawGizmo() = 0;

	DIYRigidBody*	bodyA;
	DIYRigidBody*	bodyB;

private:

};

class SpringJoint : public Joint
{
public:
	SpringJoint(DIYRigidBody* A, DIYRigidBody* B, float a_k, float a_d, float a_restingDistance);
	~SpringJoint();
	virtual	void	Update(float fDt);
	virtual	void	DrawGizmo();

	float	k;	//	spring stiffness
	float	d;	//	spring damping value
	float	fRestingdistance;

private:

};

struct CollisionManifold
{
	DIYRigidBody*	first;
	DIYRigidBody*	second;
	glm::vec2	P;
	glm::vec2	N;
	float	e;
	bool	bColliding;
};

class SphereClass: public DIYRigidBody
{
public:
	float _radius;
	SphereClass(	glm::vec2 position,glm::vec2 velocity,float mass,float radius, glm::vec4& colour);
	SphereClass(	glm::vec2 position, float angle, float speed, float radius, float mass, glm::vec4& colour);
	virtual void makeGizmo();
};

class BoxClass: public DIYRigidBody
{
public:
	float width,height;
	BoxClass(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass,float width, float height,glm::vec4& colour);
	BoxClass(	glm::vec2 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4& colour);
	virtual void makeGizmo();
	virtual	void	update(glm::vec2 gravity, float delta)
	{
		DIYRigidBody::update(gravity, delta);
		this->m_bIsColliding = false;
	};
	bool	IsPointOver(glm::vec2 point);
	bool	m_bIsColliding;
};


class DIYPhysicScene
{
	public:
	bool collisionEnabled = true;
	glm::vec2 gravity;
	float timeStep;
	std::vector<PhysicsObject*> actors;
	std::vector<Joint*> joints;
	void addActor(PhysicsObject*);
	void removeActor(PhysicsObject*);
	void addJoint(Joint*);
	void removeJoint(Joint*);
	void upDate();
	void solveIntersections();
	void debugScene();
	void upDateGizmos();
	void	CheckForCollision();
	static	CollisionManifold	Plane2Plane		(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Plane2Sphere	(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Plane2Box		(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Sphere2Plane	(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Sphere2Sphere	(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Sphere2Box		(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Box2Plane		(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Box2Sphere		(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
	static	CollisionManifold	Box2Box			(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second);
};
