#include "BBPhysicsObject.h"
#include "Gizmos.h"

///////////////////////////////////////////////////////////////////////////////////////
//	BBPhysicsObject class functions
///////////////////////////////////////////////////////////////////////////////////////
BBPhysicsObject::BBPhysicsObject()
{
}

BBPhysicsObject::~BBPhysicsObject()
{
}




///////////////////////////////////////////////////////////////////////////////////////
//	BBRigidBody class functions
///////////////////////////////////////////////////////////////////////////////////////
BBRigidBody::BBRigidBody()
{
}

BBRigidBody::~BBRigidBody()
{
}

BBRigidBody::BBRigidBody(glm::vec2 a_vPosition, glm::vec2 a_vVelocity, float a_fRotation, float a_fMass) :
m_vPosition(a_vPosition), m_vVelocity(a_vVelocity), m_fRotation2D(a_fRotation), m_fMass(a_fMass)
{
	m_fOneOnMass = 1.0f / a_fMass;
}

void	BBRigidBody::update(glm::vec2 a_fGravity, float fTimeStep)
{
	//	dv = a dt
	//	F = ma  ---->  a = F/m
	//	dv = (F/m) * dt
	vec2	vForceVelocityChange = (m_fOneOnMass * fTimeStep) * m_vForce;
	m_vVelocity += (a_fGravity * fTimeStep) + vForceVelocityChange;
	m_vPosition += (m_vVelocity * fTimeStep);

	m_vForce = vec2(0);	//	reset the force for the next timestep
}

void	BBRigidBody::debug()
{
}

void	BBRigidBody::ApplyForce(glm::vec2 a_vForce)
{
	m_vForce += a_vForce;
}

void	BBRigidBody::ApplyForceToActor(BBRigidBody*a_pActor2, glm::vec2 a_vForce)
{
	//	apply the force to the colliding actor
	a_pActor2->ApplyForce(a_vForce);
	//	then apply the negative of that force to itself as the equal and opposite reaction
	m_vForce -= a_vForce;
}


///////////////////////////////////////////////////////////////////////////////////////
//	BBSphere class functions
///////////////////////////////////////////////////////////////////////////////////////
BBSphere::BBSphere()
{
}

BBSphere::~BBSphere()
{
}

BBSphere::BBSphere(glm::vec2 a_vPosition, glm::vec2 a_vVelocity, float a_fMass, float a_fRadius, glm::vec4 a_vColour) :
BBRigidBody(a_vPosition, a_vVelocity, 0.0f, a_fMass), m_fRadius(a_fRadius), m_vColour(a_vColour)
{
	m_eShapeID = SPHERE;
}

void	BBSphere::MakeGizmo()
{
	Gizmos::addSphere(vec3(m_vPosition.x, m_vPosition.y, 0.0f), m_fRadius, 5, 6, m_vColour);
}


