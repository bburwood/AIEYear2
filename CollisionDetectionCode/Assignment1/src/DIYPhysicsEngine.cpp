#include "DIYPhysicsEngine.h"
//rigid body functions
using namespace std;

void	BuildBoxPoints(BoxClass* box, glm::vec2* points);

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
	glm::vec2 center = position.xy();
	Gizmos::add2DCircle(center, _radius, 30, colour);
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
	this->dynamicFriction = 0.1f;
	this->staticFriction = 0.1f;
	this->totalTorque = 0.0f;
	this->totalForce = glm::vec2(0.0f, 0.0f);
	this->angularVelocity = 0.0f;
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
	position += velocity * timeStep;
	velocity += vDeltaV;

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

void DIYPhysicScene::upDate()
{
	bool runPhysics = true;
	int maxIterations = 10; //emergency count to stop us repeating for ever in extreme situations

	for(auto actorPtr:actors)
	{
		actorPtr->update(gravity,timeStep);
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
	for (auto actorPtr:actors)
	{
		actorPtr->makeGizmo();
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
					float	fInvMass1 = 1.0f / manifold.first->mass;
					float	fInvMass2 = manifold.second ? 1.0f / manifold.second->mass : 0;

					float	fInvMOI1 = 1.0f / manifold.first->momentOfInertia;
					float	fInvMOI2 = manifold.second ? 1.0f / manifold.first->momentOfInertia : 0;

					glm::vec2	vCom1 = manifold.first->position;
					glm::vec2	vCom2 = manifold.second ? manifold.second->position : manifold.P;

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

					manifold.first->velocity += (j * fInvMass1) * manifold.N;
					manifold.first->angularVelocity += glm::dot(vR1P, j * manifold.N) * fInvMOI1;
					if (manifold.second)
					{
						manifold.second->velocity += (-j * fInvMass2) * manifold.N;
						manifold.second->angularVelocity += glm::dot(vR2P, -j * manifold.N) * fInvMOI2;
					}
					//	now deal with friction
					glm::vec2	vTangent(-manifold.N.y, manifold.N.x);
					float	fR1PdotT = glm::dot(vR1P, vTangent);
					float	fR2PdotT = glm::dot(vR2P, vTangent);
					float	fFrictionDenom = fInvMass1 + fInvMass2 + (fR1PdotT * fR1PdotT) * fInvMOI1 + (fR2PdotT * fR2PdotT) * fInvMOI2;
					float	fFrictionJ = (-(1.0f + manifold.e) * glm::dot(vVelocity1 - vVelocity2, vTangent)) / fFrictionDenom;

					manifold.first->velocity += (fFrictionJ * fInvMass1) * vTangent;
					manifold.first->angularVelocity += glm::dot(vR1P, fFrictionJ * vTangent) * fInvMOI1;
					if (manifold.second)
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
		sphere->position += plane->normal * fIntersection;

		result.bColliding = true;
		result.N = plane->normal;
		result.e = 0.97f;
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

		//	move circles apart
		float	fIntersection = fRadiiSum - fDistance;
		sphere1->position -= fIntersection * vCollisionNormal;
		sphere2->position += fIntersection * vCollisionNormal;

		result.e = 0.97f;
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
	result.e = 0.97f;
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
	result.e = 0.97f;
	result.first = box;
	result.second = nullptr;

	//	rotate the vector
	vVectorToCircle = glm::vec2(fCosTheta * vVectorToCircle.x - fSinTheta * vVectorToCircle.y,
								fSinTheta * vVectorToCircle.x + fCosTheta * vVectorToCircle.y);

	float	fDistSquared = 0.0f;
	if (vVectorToCircle.x > box->width)	//	if the circle is to the right
	{
		float	fDist = vVectorToCircle.x - box->width;
		fDistSquared += fDist * fDist;
	}
	else if (vVectorToCircle.x < -box->width)	//	if the circle is to the left
	{
		float	fDist = vVectorToCircle.x + box->width;
		fDistSquared += fDist * fDist;
	}
	if (vVectorToCircle.y > box->height)	//	if the circle is above
	{
		float	fDist = vVectorToCircle.y - box->height;
		fDistSquared += fDist * fDist;
	}
	else if (vVectorToCircle.y < -box->height)	//	if the circle is below
	{
		float	fDist = vVectorToCircle.y + box->height;
		fDistSquared += fDist * fDist;
	}

	if ((sphere->_radius * sphere->_radius) > fDistSquared)
	{
		box->m_bIsColliding = true;
		result.bColliding = true;
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
	result.bColliding = true;
	result.first = box1;
	result.second = box2;
	result.e = 0.97f;

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
					//result.P = ;
				}
			}
		}
		firstPoints = vPoints + 4;
		secondPoints = vPoints;
	}
	//	if we made it this far and it did not return yet, that means that they are colliding
	//	collision response
	box1->velocity = glm::vec2(0, 0);
	box2->velocity = glm::vec2(0, 0);
	box1->m_bIsColliding = true;
	box2->m_bIsColliding = true;

	return result;
}

