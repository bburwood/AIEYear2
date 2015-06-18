#include "DIYPhysicsEngine.h"
//rigid body functions
using namespace std;

void	BuildBoxPoints(BoxClass* box, glm::vec2* points);

//	function pointer array for doing our collisions
typedef	bool(*fn)(DIYPhysicScene* scene, PhysicsObject*, PhysicsObject*);
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
}

SphereClass::SphereClass(	glm::vec2 position,float angle,float speed,float radius,float mass,glm::vec4& colour)
		: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
}

void SphereClass::makeGizmo()
{
	glm::vec2 center = position.xy();
	Gizmos::add2DCircle(center, _radius,30, colour);
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
}

BoxClass::BoxClass(	glm::vec2 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4& colour)
	: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
	_shapeID = BOX;
	m_bIsColliding = false;
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

DIYRigidBody::DIYRigidBody(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass)
{
	std::cout<<"adding rigid body "<<position.x<<','<<position.y<<std::endl;
	this->position = position;
	this->velocity = velocity;
	this->rotation2D = rotation;
	this->mass = mass;
	this->dynamicFriction = 0.2f;
	this->staticFriction = 0.2f;
	colour = glm::vec4(1,1,1,1); //white by default
}

void DIYRigidBody::applyForce(glm::vec2 force)
{
		velocity += force/mass;
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

	applyForce(gravity * mass * timeStep);
	oldPosition = position; //keep our old position for collision response


	float	fNormalForce = this->mass;
	glm::vec2	vFrictionForceVector = -fNormalForce * this->dynamicFriction * this->velocity;
	glm::vec2	vFrictionAcceleration = vFrictionForceVector / this->mass;
	this->velocity += vFrictionAcceleration * timeStep;

	position += velocity * timeStep;
	rotationMatrix = glm::rotate(rotation2D,glm::vec3(0.0f,0.0f,1.0f)); 
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
				pCollisionFunctionPointer(this, object1, object2);
			}
		}
	}
}

bool	DIYPhysicScene::Plane2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return false;
}

bool	DIYPhysicScene::Plane2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Sphere2Plane(scene, second, first);
}

bool	DIYPhysicScene::Sphere2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	SphereClass*	sphere = (SphereClass*)first;
	PlaneClass*		plane = (PlaneClass*)second;

	float	fPerpendicularDist = glm::dot(sphere->position, plane->normal) - plane->distance;
	if (fPerpendicularDist < sphere->_radius)
	{
		float	fIntersection = sphere->_radius - fPerpendicularDist;
		glm::vec2	vInLineWithNormal = plane->normal * glm::dot(plane->normal, sphere->velocity);
		glm::vec2	vPerpendicular = sphere->velocity - vInLineWithNormal;

		float	fRestitution = 0.999f;
		vInLineWithNormal *= -1.0f * fRestitution;

		sphere->velocity = vPerpendicular + vInLineWithNormal;

		sphere->position += plane->normal * fIntersection;

		return true;
	}
	return false;
}

bool	DIYPhysicScene::Sphere2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	SphereClass*	sphere1 = (SphereClass*)first;
	SphereClass*	sphere2 = (SphereClass*)second;

	//	get the vector from the second to the first sphere
	glm::vec2 vDelta = sphere2->position - sphere1->position;
	//	the length^2 of the delta is the distance^2
	float	fDistance = glm::length(vDelta);
	float	fRadiiSum = sphere1->_radius + sphere2->_radius;

	if (fDistance < fRadiiSum)
	{
		//	collision reponse
		////	kill all speed
		//sphere1->velocity = glm::vec2(0);
		//sphere2->velocity = glm::vec2(0);

		//	get the normal of the collision
		glm::vec2	vCollisionNormal = glm::normalize(vDelta);

		//	split velocities into normal and non-normal components
		glm::vec2	vInLineWithNormal1 = vCollisionNormal * glm::dot(vCollisionNormal, sphere1->velocity);
		glm::vec2	vPerpendicularToNormal1 = sphere1->velocity - vInLineWithNormal1;
		glm::vec2	vInLineWithNormal2 = vCollisionNormal * glm::dot(vCollisionNormal, sphere2->velocity);
		glm::vec2	vPerpendicularToNormal2 = sphere2->velocity - vInLineWithNormal2;

		float	fRestitution = 0.999f;

		//	get mass and velocity amount in line with the normal
		float	u1 = glm::dot(vCollisionNormal, sphere1->velocity);
		float	m1 = sphere1->mass;
		float	u2 = glm::dot(vCollisionNormal, sphere2->velocity);
		float	m2 = sphere2->mass;

		//float	u = u1 - u2;
		//	conservation of momentum equation!
		float	v1 = (u1 * (m1 - m2) / (m1 + m2)) + (u2 * (2 * m2) / (m1 + m2));
		float	v2 = (u2 * (m2 - m1) / (m1 + m2)) + (u1 * (2 * m1) / (m1 + m2));
		//float	v1 = (u * (m1 - m2)) / (m1 + m2);
		//float	v2 = (u * (2 * m1)) / (m1 + m2);

		//v1 += u2;
		//v2 += u2;

		v1 *= fRestitution;
		v2 *= fRestitution;

		//	rebuild velocity vectors
		glm::vec2	vNewInLineWithNormal1 = vCollisionNormal * v1;
		glm::vec2	vNewInLineWithNormal2 = vCollisionNormal * v2;

		glm::vec2	vNewV1 = vNewInLineWithNormal1 + vPerpendicularToNormal1;
		glm::vec2	vNewV2 = vNewInLineWithNormal2 + vPerpendicularToNormal2;

		sphere1->velocity = vNewV1;
		sphere2->velocity = vNewV2;

		//	move circles apart
		float	fIntersection = fRadiiSum - fDistance;
		sphere1->position -= fIntersection * vCollisionNormal;
		sphere2->position += fIntersection * vCollisionNormal;

		return true;
	}
	return false;
}

bool	DIYPhysicScene::Plane2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Box2Plane(scene, second, first);
}

bool	DIYPhysicScene::Sphere2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Box2Sphere(scene, second, first);
}

bool	DIYPhysicScene::Box2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box = (BoxClass*)first;
	PlaneClass*	plane = (PlaneClass*)second;

	glm::vec2	vPoints[4];
	BuildBoxPoints(box, vPoints);

	int	iPointSides = 0;
	for (int iPointIndex = 0; iPointIndex < 4; ++iPointIndex)
	{
		float	fDistance = glm::dot(plane->normal, vPoints[iPointIndex]) - plane->distance;
		if (fDistance > 0)
		{
			++iPointSides;
		}
		else
		{
			--iPointSides;
		}
	}
	if ((iPointSides == 4) || (iPointSides == -4))
	{
		return false;
	}
	box->m_bIsColliding = true;
	return true;
}

bool	DIYPhysicScene::Box2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box = (BoxClass*)first;
	SphereClass*	sphere = (SphereClass*)second;

	glm::vec2	vVectorToCircle = sphere->position - box->position;

	float	fSinTheta = sinf(-box->rotation2D);
	float	fCosTheta = cosf(-box->rotation2D);

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
		return true;
	}
	return false;
}

void	BuildBoxPoints(BoxClass* box, glm::vec2* points)
{
	points[0] = (box->rotationMatrix * glm::vec4(-box->width, -box->height, 0, 1)).xy() + box->position;
	points[1] = (box->rotationMatrix * glm::vec4(-box->width, box->height, 0, 1)).xy() + box->position;
	points[2] = (box->rotationMatrix * glm::vec4(box->width, box->height, 0, 1)).xy() + box->position;
	points[3] = (box->rotationMatrix * glm::vec4(box->width, -box->height, 0, 1)).xy() + box->position;
}

bool	DIYPhysicScene::Box2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass*	box1 = (BoxClass*)first;
	BoxClass*	box2 = (BoxClass*)second;

	glm::vec2	vPoints[8];
	glm::vec2*	firstPoints = vPoints;
	glm::vec2*	secondPoints = vPoints + 4;

	//	first we need to get the 4 vertices of each box
	BuildBoxPoints(box1, vPoints);
	BuildBoxPoints(box2, vPoints + 4);

	float	minOverlap;

	for (int boxIndex = 0; boxIndex < 2; ++boxIndex)
	{
		for (int pointIndex = 0; pointIndex < 3; ++pointIndex)
		{
			glm::vec2	edgeVector = firstPoints[pointIndex] - firstPoints[pointIndex + 1];
			edgeVector = glm::normalize(edgeVector);
			glm::vec2	perpVector(edgeVector.y, -edgeVector.x);
			float	firstMin = FLT_MAX, firstMax = -FLT_MAX;
			float	secondMin = FLT_MAX, secondMax = -FLT_MAX;
			for (int checkIndex = 0; checkIndex < 4; ++checkIndex)
			{
				float	firstProjected = glm::dot(firstPoints[checkIndex], perpVector);
				if (firstProjected < firstMin)
				{
					firstMin = firstProjected;
				}
				if (firstProjected > firstMax)
				{
					firstMax = firstProjected;
				}
				float	secondProjected = glm::dot(secondPoints[checkIndex], perpVector);
				if (secondProjected < secondMin)
				{
					secondMin = secondProjected;
				}
				if (secondProjected > secondMax)
				{
					secondMax = secondProjected;
				}
			}
			if ((firstMin > secondMax) || (secondMin > firstMax))
			{
				return false;
			}
			else
			{

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

	return true;
}

