#include "DIYPhysicsEngine.h"
#include "Gizmos.h"

//rigid body functions
using namespace std;

void	BuildBoxPoints(BoxClass* box, glm::vec2* points);

float	fCollisionEValue = 0.8f;
float	g_fDynamicFriction = 0.25f;
float	g_fStaticFriction = 0.6f;

//	function pointer array for doing our collisions
typedef	CollisionManifold	(*fn)(DIYPhysicScene* scene, PhysicsObject*, PhysicsObject*);
static	fn	CollisionFunctionArray[] =
{
	DIYPhysicScene::Plane2Plane, DIYPhysicScene::Plane2Sphere, DIYPhysicScene::Plane2Box,
	DIYPhysicScene::Sphere2Plane, DIYPhysicScene::Sphere2Sphere, DIYPhysicScene::Sphere2Box,
	DIYPhysicScene::Box2Plane, DIYPhysicScene::Box2Sphere, DIYPhysicScene::Box2Box,
};

//plane class functions

PlaneClass::PlaneClass(glm::vec2 normal,float distance)
{
	this->normal = glm::normalize(normal);
	this->distance = distance;
	
	_shapeID = PLANE;
}

void PlaneClass::makeGizmo()
{
	float lineSegmentLength = 300;
	glm::vec2 centrePoint = normal * distance;
	glm::vec2 parallel = glm::vec2(normal.y,-normal.x); //easy to rotate normal through 90degrees around z
	glm::vec4 colour(1,1,1,1);
	glm::vec2 start = centrePoint + (parallel * lineSegmentLength);
	glm::vec2 end = centrePoint - (parallel * lineSegmentLength);
	Gizmos::add2DLine(start.xy(),end.xy(),colour);
}

//sphere class functions

SphereClass::SphereClass(	glm::vec2 position,glm::vec2 velocity,float radius,float mass,glm::vec4& colour)
	: DIYRigidBody(position,velocity,0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
	this->momentOfInertia = this->mass * this->_radius * this->_radius * 0.5f;
}

SphereClass::SphereClass(	glm::vec2 position,float angle,float speed,float radius,float mass,glm::vec4& colour)
		: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
	this->momentOfInertia = this->mass * this->_radius * this->_radius * 0.5f;
}

void SphereClass::makeGizmo()
{
	glm::vec2 centre = position.xy();
	Gizmos::add2DCircle(centre, _radius, 30, colour);
	//	now add in visualising the rotation as well!
	//	get the vector to the edge in the rotation angle
	glm::vec2	vDirection(_radius, 0.0f);
	vDirection = glm::rotate(vDirection, rotation2D);
	if (colour == glm::vec4(1, 1, 1, 1))
	{
		Gizmos::add2DLine(centre, centre + vDirection, glm::vec4(0.5f, 0.5f, 0.5f, 1));
	}
	else
	{
		Gizmos::add2DLine(centre, centre + vDirection, glm::vec4(1, 1, 1, 1));
	}
}

bool	SphereClass::IsPointOver(glm::vec2 point)
{
	glm::vec2	vRelPoint = point - this->position;

	bool	bResult = (glm::length2(vRelPoint) < this->_radius * this->_radius);
	return bResult;
}

//box class functions

BoxClass::BoxClass(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass,float width, float height,glm::vec4& colour)
	: DIYRigidBody(position,velocity,rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
	_shapeID = BOX;
	m_bIsColliding = false;
	float	h = 2.0f * height;
	float	w = 2.0f * width;
	this->momentOfInertia = (mass * ((h * h) + (w * w))) / 12.0f;
}

BoxClass::BoxClass(	glm::vec2 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4& colour)
	: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
	_shapeID = BOX;
	m_bIsColliding = false;
	float	h = 2.0f * height;
	float	w = 2.0f * width;
	this->momentOfInertia = (mass * ((h * h) + (w * w))) / 12.0f;
}

void BoxClass::makeGizmo()
{
	//Get a rotation matrix from our 2D rotation so we can draw our gizmo
	glm::vec4	vMyColour;
	if (m_bIsColliding)
	{
		vMyColour = glm::vec4(1, 0, 0, 1);
	}
	else
	{
		vMyColour = glm::vec4(0, 0, 1, 1);
	}

	Gizmos::add2DAABBFilled(position, glm::vec2(width, height), vMyColour, &rotationMatrix);
}

bool	BoxClass::IsPointOver(glm::vec2 point)
{
	glm::vec2	vRelPoint = point - position;
	float	fCosTh = cosf(-rotation2D);
	float	fSinTh = sinf(-rotation2D);

	vRelPoint = glm::vec2(fCosTh * vRelPoint.x - fSinTh * vRelPoint.y,
							fSinTh * vRelPoint.x + fCosTh * vRelPoint.y);

	bool	bResult = (vRelPoint.x > -width) && (vRelPoint.x < width) && (vRelPoint.y > -height) && (vRelPoint.y < height);
	return bResult;
}

DIYRigidBody::DIYRigidBody(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass)
{
	std::cout<<"adding rigid body "<<position.x<<','<<position.y<<std::endl;
	this->position = position;
	this->velocity = velocity;
	this->rotation2D = rotation;
	this->mass = mass;
	this->dynamicFriction = g_fDynamicFriction;
	this->staticFriction = g_fStaticFriction;
	this->totalTorque = 0.0f;
	this->totalForce = glm::vec2(0.0f, 0.0f);
	this->angularVelocity = 0.0f;
	this->bIsStatic = false;
	colour = glm::vec4(1,1,1,1); //white by default
}

void DIYRigidBody::applyForce(glm::vec2 force)
{
	applyForceAtPoint(force, this->position);
}

void	DIYRigidBody::applyForceAtPoint(glm::vec2 force, glm::vec2 point)
{
	totalForce += force;
	glm::vec2	cmToPoint = point - this->position;
	glm::vec2	perpToPoint(-cmToPoint.y, cmToPoint.x);
	totalTorque += glm::dot(perpToPoint, force);
}

void DIYRigidBody::applyForceToActor(DIYRigidBody* actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	applyForce(-force);
}

void DIYRigidBody::collisionResponse(glm::vec2 collisionPoint)
{
	// for now we'll just set the velocity to zero
	velocity = glm::vec2(0,0);
	collisionPoint = collisionPoint;
}

void DIYRigidBody::update(glm::vec2 gravity,float timeStep)
{
	if (bIsStatic)
	{
		return;
	}
	applyForce(gravity * mass);

	float	fNormalForce = this->mass;
	glm::vec2	vFrictionForceVector = -this->mass * this->dynamicFriction * this->velocity;
	applyForce(vFrictionForceVector);

	float	fFrictionTorque = -momentOfInertia * this->dynamicFriction * angularVelocity;
	totalTorque += fFrictionTorque;

	glm::vec2	vAcceleration = (totalForce / mass);
	glm::vec2	vDeltaV = vAcceleration * timeStep;

	float	fAngularAcceleration = totalTorque / momentOfInertia;
	float	fDeltaW = fAngularAcceleration * timeStep;

	oldPosition = position; //keep our old position for collision response
	velocity += vDeltaV;
	position += velocity * timeStep;

	rotation2D += angularVelocity * timeStep;
	angularVelocity += fDeltaW;

	glm::vec2	vFrictionAcceleration = vFrictionForceVector / this->mass;
	this->velocity += vFrictionAcceleration * timeStep;

	rotationMatrix = glm::rotate(rotation2D,glm::vec3(0.0f,0.0f,1.0f));

	totalForce = glm::vec2(0.0f, 0.0f);
	totalTorque = 0.0f;
}

void DIYRigidBody::debug()
{
	cout<<"position "<<position.x<<','<<position.y<<endl;
}

//scene functions

void DIYPhysicScene::addActor(PhysicsObject* object)
{
	actors.push_back(object);
}

void DIYPhysicScene::removeActor(PhysicsObject* object)
{
	auto item = std::find(actors.begin(), actors.end(), object);
	if(item < actors.end())
	{
		actors.erase(item);
	}
}

void DIYPhysicScene::addJoint(Joint* object)
{
	joints.push_back(object);
}

void DIYPhysicScene::removeJoint(Joint* object)
{
	auto item = std::find(joints.begin(), joints.end(), object);
	if (item < joints.end())
	{
		joints.erase(item);
	}
}

void DIYPhysicScene::upDate()
{
	bool runPhysics = true;
	int maxIterations = 10; //emergency count to stop us repeating for ever in extreme situations

	for (auto actorPtr : actors)
	{
		actorPtr->update(gravity, timeStep);
	}
	for (auto jointPtr : joints)
	{
		jointPtr->Update(timeStep);
	}
	if (collisionEnabled)
	{
		CheckForCollision();
	}
	maxIterations--;
}

void DIYPhysicScene::debugScene()
{
	int count = 0;
	for (auto actorPtr:actors)
	{
		cout<<count<<" : ";
		actorPtr->debug();
		count++;
	}
}

void DIYPhysicScene::upDateGizmos()
{
	for (auto actorPtr : actors)
	{
		actorPtr->makeGizmo();
	}
	for (auto jointPtr : joints)
	{
		jointPtr->DrawGizmo();
	}
}

void	DIYPhysicScene::CheckForCollision()
{
	int	iActorCount = actors.size();
	//	need to check collisions against all objects except this one
	for (int outer = 0; outer < iActorCount - 1; ++outer)
	{
		for (int inner = outer + 1; inner < iActorCount; ++inner)
		{
			PhysicsObject*	object1 = actors[outer];
			PhysicsObject*	object2 = actors[inner];
			int	iShapeID1 = object1->_shapeID;
			int	iShapeID2 = object2->_shapeID;
			//	using function pointers
			int	iFunctionIndex = (iShapeID1 * NUMBERSHAPE) + iShapeID2;
			fn pCollisionFunctionPointer = CollisionFunctionArray[iFunctionIndex];
			if (pCollisionFunctionPointer != nullptr)
			{
				CollisionManifold	manifold = pCollisionFunctionPointer(this, object1, object2);
				if (manifold.bColliding)
				{
					if (manifold.first->bIsStatic && manifold.second && manifold.second->bIsStatic)
					{
						continue;
					}
					//	draw a debug gizmo
					Gizmos::add2DLine(manifold.P, manifold.P + (manifold.N * 15.0f), glm::vec4(1, 1, 1, 1));

					float	fInvMass1 = (!manifold.first->bIsStatic) ? 1.0f / manifold.first->mass : 0;
					float	fInvMass2 = (manifold.second && !manifold.second->bIsStatic) ? 1.0f / manifold.second->mass : 0;

					float	fInvMOI1 = (!manifold.first->bIsStatic) ? 1.0f / manifold.first->momentOfInertia : 0;
					float	fInvMOI2 = manifold.second ? 1.0f / manifold.second->momentOfInertia : 0;

					glm::vec2	vCom1 = manifold.first->position;
					glm::vec2	vCom2 = (manifold.second && !manifold.second->bIsStatic) ? manifold.second->position : manifold.P;

					glm::vec2	vR1P = manifold.P - manifold.first->position;
					vR1P = glm::vec2(-vR1P.y, vR1P.x);
					glm::vec2	vR2P = manifold.P - (manifold.second ? manifold.second->position : manifold.P);
					vR2P = glm::vec2(-vR2P.y, vR2P.x);
					glm::vec2	vVelocity1 = manifold.first->velocity + manifold.first->angularVelocity * vR1P;
					glm::vec2	vVelocity2;
					if (manifold.second)
					{
						vVelocity2 = manifold.second->velocity + manifold.second->angularVelocity * vR2P;
					}
					float	R1PdotN = glm::dot(vR1P, manifold.N);
					float	R2PdotN = glm::dot(vR2P, manifold.N);
					float	fDenom = glm::dot(manifold.N, manifold.N * (fInvMass1 + fInvMass2)) + R1PdotN * R1PdotN * fInvMOI1 + R2PdotN * R2PdotN * fInvMOI2;
					float	j = (-(1.0f + manifold.e) * glm::dot(vVelocity1 - vVelocity2, manifold.N)) / fDenom;

					if (!manifold.first->bIsStatic)
					{
						manifold.first->velocity += (j * fInvMass1) * manifold.N;
						manifold.first->angularVelocity += glm::dot(vR1P, j * manifold.N) * fInvMOI1;
					}
					if (manifold.second && !manifold.second->bIsStatic)
					{
						manifold.second->velocity += (-j * fInvMass2) * manifold.N;
						manifold.second->angularVelocity += glm::dot(vR2P, -j * manifold.N) * fInvMOI2;
					}
					//	now deal with friction
					glm::vec2	vTangent(-manifold.N.y, manifold.N.x);
					float	fR1PdotT = glm::dot(vR1P, vTangent);
					float	fR2PdotT = glm::dot(vR2P, vTangent);
					float	fFrictionDenom = fInvMass1 + fInvMass2 + (fR1PdotT * fR1PdotT) * fInvMOI1 + (fR2PdotT * fR2PdotT) * fInvMOI2;
					float	fTempCoeffOfFriction = manifold.first->staticFriction;
					if (manifold.second)
					{
						//	average the coefficients of friction
						fTempCoeffOfFriction = (fTempCoeffOfFriction + manifold.second->staticFriction) * 0.5f;
					}
					float	fFrictionJ = (-(fTempCoeffOfFriction * manifold.e) * glm::dot(vVelocity1 - vVelocity2, vTangent)) / fFrictionDenom;

					if (!manifold.first->bIsStatic)
					{
						manifold.first->velocity += (fFrictionJ * fInvMass1) * vTangent;
						manifold.first->angularVelocity += glm::dot(vR1P, fFrictionJ * vTangent) * fInvMOI1;
					}
					if (manifold.second && !manifold.second->bIsStatic)
					{
						manifold.second->velocity += (-fFrictionJ * fInvMass2) * vTangent;
						manifold.second->angularVelocity += glm::dot(vR2P, -fFrictionJ * vTangent) * fInvMOI2;
					}
				}
			}
		}
	}
}

CollisionManifold	DIYPhysicScene::Plane2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	CollisionManifold	result = {};
	result.bColliding = false;
	return result;
}

CollisionManifold	DIYPhysicScene::Plane2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Sphere2Plane(scene, second, first);
}

CollisionManifold	DIYPhysicScene::Sphere2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	SphereClass*	sphere = (SphereClass*)first;
	PlaneClass*		plane = (PlaneClass*)second;

	float	fPerpendicularDist = glm::dot(sphere->position, plane->normal) - plane->distance;

	CollisionManifold	result;
	result.bColliding = false;
	result.first = sphere;
	result.second = nullptr;

	if (fPerpendicularDist < sphere->_radius)
	{
		float	fIntersection = sphere->_radius - fPerpendicularDist;
		glm::vec2	vForceVector = -sphere->mass * plane->normal * glm::dot(plane->normal, sphere->velocity);
		sphere->applyForce(2.0f * vForceVector);
		sphere->position += plane->normal * fIntersection;

		result.bColliding = true;
		result.N = plane->normal;
		result.e = fCollisionEValue;
		result.P = sphere->position - plane->normal * sphere->_radius;
	}
	return result;
}

CollisionManifold	DIYPhysicScene::Sphere2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	SphereClass*	sphere1 = (SphereClass*)first;
	SphereClass*	sphere2 = (SphereClass*)second;

	//	get the vector from the second to the first sphere
	glm::vec2 vDelta = sphere2->position - sphere1->position;
	//	the length^2 of the delta is the distance^2
	float	fDistance = glm::length(vDelta);
	float	fRadiiSum = sphere1->_radius + sphere2->_radius;

	CollisionManifold	result = {};
	result.first = sphere1;
	result.second = sphere2;
	result.bColliding = false;

	if (fDistance < fRadiiSum)
	{
		//	get the normal of the collision
		glm::vec2	vCollisionNormal = glm::normalize(vDelta);
		glm::vec2	vRelativeVelocity = sphere1->velocity - sphere2->velocity;
		glm::vec2	vCollisionVector = vCollisionNormal * glm::dot(vRelativeVelocity, vCollisionNormal);
		glm::vec2	vForceVector = vCollisionVector / (1.0f / sphere1->mass + 1.0f / sphere2->mass);
		sphere1->applyForceToActor(sphere2, 2.0f * vForceVector);

		//	move circles apart
		float	fIntersection = (fRadiiSum - fDistance) * 0.5f;
		if (sphere1->bIsStatic || sphere2->bIsStatic)
		{
			fIntersection *= 2.0f;
		}
		if (!sphere1->bIsStatic)
		{
			sphere1->position -= fIntersection * vCollisionNormal;
		}
		if (!sphere2->bIsStatic)
		{
			sphere2->position += fIntersection * vCollisionNormal;
		}

		result.e = fCollisionEValue;
		result.N = vCollisionNormal;
		result.P = sphere1->position + vCollisionNormal * sphere1->_radius;
		result.bColliding = true;
	}
	return result;
}

CollisionManifold	DIYPhysicScene::Plane2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Box2Plane(scene, second, first);
}

CollisionManifold	DIYPhysicScene::Sphere2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Box2Sphere(scene, second, first);
}

CollisionManifold	DIYPhysicScene::Box2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box = (BoxClass*)first;
	PlaneClass*	plane = (PlaneClass*)second;

	glm::vec2	vPoints[4];
	BuildBoxPoints(box, vPoints);

	CollisionManifold	result = {};
	result.bColliding = false;
	result.e = fCollisionEValue;
	result.first = box;
	result.second = nullptr;

	float	fLowestDistance = 0.0f;
	glm::vec2	vLowestPoint;
	for (int iPointIndex = 0; iPointIndex < 4; ++iPointIndex)
	{
		float	fDistance = glm::dot(plane->normal, vPoints[iPointIndex]) - plane->distance;
		if (fDistance < 0)
		{
			result.bColliding = true;
			result.N = plane->normal;

			if (fDistance < fLowestDistance)
			{
				fLowestDistance = fDistance;
				result.P = vPoints[iPointIndex] + plane->normal * fLowestDistance;
			}
		}
	}
	if (result.bColliding)
	{
		box->m_bIsColliding = true;
		box->position -= plane->normal * fLowestDistance;
	}
	return result;
}

CollisionManifold	DIYPhysicScene::Box2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box = (BoxClass*)first;
	SphereClass*	sphere = (SphereClass*)second;

	glm::vec2	vVectorToCircle = sphere->position - box->position;

	float	fSinTheta = sinf(-box->rotation2D);
	float	fCosTheta = cosf(-box->rotation2D);

	CollisionManifold	result = {};
	result.bColliding = false;
	result.e = fCollisionEValue;
	result.first = box;
	result.second = nullptr;

	//	rotate the vector
	vVectorToCircle = glm::vec2(fCosTheta * vVectorToCircle.x - fSinTheta * vVectorToCircle.y,
								fSinTheta * vVectorToCircle.x + fCosTheta * vVectorToCircle.y);

	bool	bLeft = false;
	bool	bRight = false;
	bool	bAbove = false;
	bool	bBelow = false;
	float	fDistSquared = 0.0f;
	if (vVectorToCircle.x > box->width)	//	if the circle is to the right
	{
		float	fDist = vVectorToCircle.x - box->width;
		fDistSquared += fDist * fDist;
		bRight = true;
	}
	else if (vVectorToCircle.x < -box->width)	//	if the circle is to the left
	{
		float	fDist = vVectorToCircle.x + box->width;
		fDistSquared += fDist * fDist;
		bLeft = true;
	}
	if (vVectorToCircle.y > box->height)	//	if the circle is above
	{
		float	fDist = vVectorToCircle.y - box->height;
		fDistSquared += fDist * fDist;
		bAbove = true;
	}
	else if (vVectorToCircle.y < -box->height)	//	if the circle is below
	{
		float	fDist = vVectorToCircle.y + box->height;
		fDistSquared += fDist * fDist;
		bBelow = true;
	}

	if ((sphere->_radius * sphere->_radius) > fDistSquared)
	{
		box->m_bIsColliding = true;
		result.bColliding = true;
		result.second = sphere;
		//	now need to calculate the contact point, collision normal, and intersection distance
		glm::vec2	vTempP;
		glm::vec2	vTempN;
		float	fIntersection = 0.00001f;	//	initialise to a small value
		if (!bLeft && !bRight && bAbove)
		{
			//	contact was made with the top edge of the box
			vTempP.x = vVectorToCircle.x;
			vTempP.y = box->height;
			vTempN = glm::vec2(0, 1);
			fIntersection = glm::abs(vVectorToCircle.y - sphere->_radius - box->height);
		}
		else if (!bLeft && !bRight && bBelow)
		{
			//	contact was made with the bottom edge of the box
			vTempP.x = vVectorToCircle.x;
			vTempP.y = -box->height;
			vTempN = glm::vec2(0, -1);
			fIntersection = glm::abs(vVectorToCircle.y + sphere->_radius + box->height);
		}
		else if (bRight && !bBelow && !bAbove)
		{
			//	contact was made with the right edge of the box
			vTempP.x = box->width;
			vTempP.y = vVectorToCircle.y;
			vTempN = glm::vec2(1, 0);
			fIntersection = glm::abs(vVectorToCircle.x - sphere->_radius - box->width);
		}
		else if (bLeft && !bBelow && !bAbove)
		{
			//	contact was made with the left edge of the box
			vTempP.x = -box->width;
			vTempP.y = vVectorToCircle.y;
			vTempN = glm::vec2(-1, 0);
			fIntersection = glm::abs(vVectorToCircle.x + sphere->_radius + box->width);
		}
		//	else contact was made with a corner, work out which one
		else if (bRight && bAbove)
		{
			//	contact with top right corner
			vTempP.x = box->width;
			vTempP.y = box->height;
			//	now get the normal between the contact point and circle centre
			//	for this I will use as an approximation, simply the normalised vector from the corner of the box to the center of the sphere
			vTempN = glm::normalize(vVectorToCircle - vTempP);
			fIntersection = sphere->_radius - (vVectorToCircle - vTempP).length();
		}
		else if (bRight && bBelow)
		{
			//	contact with bottom right corner
			vTempP.x = box->width;
			vTempP.y = -box->height;
			//	now get the normal between the contact point and circle centre
			//	for this I will use as an approximation, simply the normalised vector from the corner of the box to the center of the sphere
			vTempN = glm::normalize(vVectorToCircle - vTempP);
			fIntersection = sphere->_radius - (vVectorToCircle - vTempP).length();
		}
		else if (bLeft && bAbove)
		{
			//	contact with top left corner
			vTempP.x = -box->width;
			vTempP.y = box->height;
			//	now get the normal between the contact point and circle centre
			//	for this I will use as an approximation, simply the normalised vector from the corner of the box to the center of the sphere
			vTempN = glm::normalize(vVectorToCircle - vTempP);
			fIntersection = sphere->_radius - (vVectorToCircle - vTempP).length();
		}
		else if (bLeft && bBelow)
		{
			//	contact with bottom left corner
			vTempP.x = -box->width;
			vTempP.y = -box->height;
			//	now get the normal between the contact point and circle centre
			//	for this I will use as an approximation, simply the normalised vector from the corner of the box to the center of the sphere
			vTempN = glm::normalize(vVectorToCircle - vTempP);
			fIntersection = sphere->_radius - (vVectorToCircle - vTempP).length();
		}
		//	need to re-rotate the contact point and normal
		fSinTheta = sinf(box->rotation2D);
		fCosTheta = cosf(box->rotation2D);
		//	rotate the contact point
		result.P = glm::vec2(fCosTheta * vTempP.x - fSinTheta * vTempP.y,
			fSinTheta * vTempP.x + fCosTheta * vTempP.y);
		//	add the box position back into it
		result.P += box->position;
		//	rotate the normal
		result.N = glm::vec2(fCosTheta * vTempN.x - fSinTheta * vTempN.y,
			fSinTheta * vTempN.x + fCosTheta * vTempN.y);
		
		//	find the force to add to the objects
		glm::vec2	vRelativeVelocity = sphere->velocity - box->velocity;
		glm::vec2	vCollisionVector = result.N * glm::dot(vRelativeVelocity, result.N);
		glm::vec2	vForceVector = 2.0f * vCollisionVector / (1.0f / sphere->mass + 1.0f / box->mass);

		//	now move them apart
		if (sphere->bIsStatic || box->bIsStatic)
		{
			fIntersection *= 2.0f;
		}
		if (!sphere->bIsStatic)
		{
			sphere->position += fIntersection * result.N;
			sphere->applyForce(vForceVector);
		}
		if (!box->bIsStatic)
		{
			box->position -= fIntersection * result.N;
			box->applyForce(vForceVector);
		}

		return result;
	}
	return result;
}

void	BuildBoxPoints(BoxClass* box, glm::vec2* points)
{
	points[0] = (box->rotationMatrix * glm::vec4(-box->width, -box->height, 0, 1)).xy() + box->position;
	points[1] = (box->rotationMatrix * glm::vec4(-box->width, box->height, 0, 1)).xy() + box->position;
	points[2] = (box->rotationMatrix * glm::vec4(box->width, box->height, 0, 1)).xy() + box->position;
	points[3] = (box->rotationMatrix * glm::vec4(box->width, -box->height, 0, 1)).xy() + box->position;
}

CollisionManifold	DIYPhysicScene::Box2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box1 = (BoxClass*)first;
	BoxClass*	box2 = (BoxClass*)second;

	glm::vec2	vPoints[8];
	glm::vec2*	firstPoints = vPoints;
	glm::vec2*	secondPoints = vPoints + 4;

	//	first we need to get the 4 vertices of each box
	BuildBoxPoints(box1, vPoints);
	BuildBoxPoints(box2, vPoints + 4);

	CollisionManifold	result = {};
	result.bColliding = false;
	result.first = box1;
	result.second = box2;
	result.e = fCollisionEValue;

	glm::vec2	vSmallestNormal;
	float	minOverlap = FLT_MAX;

	for (int boxIndex = 0; boxIndex < 2; ++boxIndex)
	{
		for (int pointIndex = 0; pointIndex < 3; ++pointIndex)
		{
			glm::vec2	edgeVector = firstPoints[pointIndex] - firstPoints[pointIndex + 1];
			edgeVector = glm::normalize(edgeVector);
			glm::vec2	perpVector(edgeVector.y, -edgeVector.x);
			float	firstMin = FLT_MAX, firstMax = -FLT_MAX;
			float	secondMin = FLT_MAX, secondMax = -FLT_MAX;

			glm::vec2	vFirstMinPoint, vFirstMaxPoint;
			glm::vec2	vSecondMinPoint, vSecondMaxPoint;

			for (int checkIndex = 0; checkIndex < 4; ++checkIndex)
			{
				float	firstProjected = glm::dot(firstPoints[checkIndex], perpVector);
				if (firstProjected < firstMin)
				{
					firstMin = firstProjected;
					vFirstMinPoint = firstPoints[checkIndex];
				}
				if (firstProjected > firstMax)
				{
					firstMax = firstProjected;
					vFirstMaxPoint = firstPoints[checkIndex];
				}
				float	secondProjected = glm::dot(secondPoints[checkIndex], perpVector);
				if (secondProjected < secondMin)
				{
					secondMin = secondProjected;
					vSecondMinPoint = secondPoints[checkIndex];
				}
				if (secondProjected > secondMax)
				{
					secondMax = secondProjected;
					vSecondMaxPoint = secondPoints[checkIndex];
				}
			}
			if ((firstMin > secondMax) || (secondMin > firstMax))
			{
				return result;	//	colliding will be false
			}
			else
			{
				float	max1_min0 = secondMax - firstMin;
				float	max0_min1 = firstMax - secondMin;
				float	fOverlap = glm::min(max1_min0, max0_min1);
				if (fOverlap < minOverlap)
				{
					result.N = perpVector;
					minOverlap = fOverlap;
					//result.P = ;
				}
			}
		}
		firstPoints = vPoints + 4;
		secondPoints = vPoints;
	}
	//	if we made it this far and it did not return yet, that means that they are colliding
	//	collision response
	//	for most situations only a single point of 1 of the boxes will be overlapping the other box, so find the first overlapping point and use it as the point of contact
	//	check the points of each box against the other box
	float	fDirChange = 1.0f;
	for (int pointIndex = 0; pointIndex < 8; ++pointIndex)
	{
		if (pointIndex < 4)
		{
			//	then we are checking points of the first box
			if (box2->IsPointOver(vPoints[pointIndex]))
			{
				//	we have found an overlapping point
				result.P = vPoints[pointIndex] + (0.5f * minOverlap * result.N);
				fDirChange = -1.0f;
				break;
			}
		}
		else
		{
			//	then we are checking points of the second box
			if (box1->IsPointOver(vPoints[pointIndex]))
			{
				//	we have found an overlapping point
				result.P = vPoints[pointIndex] - (0.5f * minOverlap * result.N);
				break;
			}
		}
	}
	//	as a test add a gizmo to draw the point ... maybe
	//	box1->velocity = glm::vec2(0, 0);
	//	box2->velocity = glm::vec2(0, 0);
	box1->m_bIsColliding = true;
	box2->m_bIsColliding = true;
	result.bColliding = true;
	if (box1->bIsStatic || box2->bIsStatic)
	{
		minOverlap *= 2.0f;
	}
	//	now move the boxes back along the collision normal
	if (!box1->bIsStatic)
	{
		box1->position -= (0.5f * fDirChange * minOverlap * result.N);
	}
	if (!box2->bIsStatic)
	{
		box2->position += (0.5f * fDirChange * minOverlap * result.N);
	}

	return result;
}

SpringJoint::SpringJoint(DIYRigidBody* a_A, DIYRigidBody* a_B, float a_k, float a_d, float a_restingDistance, glm::vec4 a_colour)
{
	this->bodyA = a_A;
	this->bodyB = a_B;
	this->k = a_k;
	this->d = a_d;
	this->fRestingdistance = a_restingDistance;
	this->jointColour = a_colour;
}

SpringJoint::~SpringJoint()
{
}

Joint::Joint()
{
}

Joint::~Joint()
{
}

void	SpringJoint::Update(float fDt)
{
	if (!bodyA || !bodyB)
	{
		return;
	}
	glm::vec2	vDiffVector = bodyA->position - bodyB->position;	//	vector from B to A
	float	fDistance = glm::length(vDiffVector);
	glm::vec2	vForce = -k * (fDistance - fRestingdistance) * glm::normalize(vDiffVector);
	if (!bodyA->bIsStatic)
	{
		bodyA->applyForce(vForce - (d * bodyA->velocity));
	}
	if (!bodyB->bIsStatic)
	{
		bodyB->applyForce(-vForce - (d * bodyB->velocity));
	}
}

void	SpringJoint::DrawGizmo()
{
	Gizmos::add2DLine(bodyA->position, bodyB->position, jointColour);
}
