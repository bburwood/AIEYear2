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
	float mass;
	float rotation2D; //2D so we only need a single float to represent our rotation about Z
	glm::mat4 rotationMatrix;
	DIYRigidBody(glm::vec2 position,glm::vec2 velocity,float rotation,float mass);
	virtual void update(glm::vec2 gravity,float timeStep);
	virtual void debug();
	virtual void collisionResponse(glm::vec2 collisionPoint);
	virtual void resetPosition(){position = oldPosition;};
	void applyForce(glm::vec2 force);
	void applyForceToActor(DIYRigidBody* actor2, glm::vec2 force);

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
};


class DIYPhysicScene
{
	public:
	bool collisionEnabled = true;
	glm::vec2 gravity;
	float timeStep;
	std::vector<PhysicsObject*> actors;
	void addActor(PhysicsObject*);
	void removeActor(PhysicsObject*);
	void upDate();
	void solveIntersections();
	void debugScene();
	void upDateGizmos();
	void	CheckForCollision();
	static	bool	Plane2Plane(PhysicsObject* first, PhysicsObject* second);
	static	bool	Plane2Sphere(PhysicsObject* first, PhysicsObject* second);
	static	bool	Plane2Box(PhysicsObject* first, PhysicsObject* second);
	static	bool	Sphere2Plane(PhysicsObject* first, PhysicsObject* second);
	static	bool	Sphere2Sphere(PhysicsObject* first, PhysicsObject* second);
	static	bool	Sphere2Box(PhysicsObject* first, PhysicsObject* second);
	static	bool	Box2Plane(PhysicsObject* first, PhysicsObject* second);
	static	bool	Box2Sphere(PhysicsObject* first, PhysicsObject* second);
	static	bool	Box2Box(PhysicsObject* first, PhysicsObject* second);
};
