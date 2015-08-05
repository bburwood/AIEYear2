#include "MyController.h"
#include <iostream>

MyControllerHitReport::~MyControllerHitReport()
{
}

void	MyControllerHitReport::onShapeHit(const PxControllerShapeHit &hit)
{
	//	gets a reference to a structure which tells us what has been hit and where
	//	get the actor from the shape we hit
	PxRigidActor*	actor = hit.shape->getActor();
	//	get the normal of the thing we hit and store it so the player controller can respond correctly
	_playerContactNormal = hit.worldNormal;
	//	try to cast to a dynamic actor
	PxRigidDynamic*	myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{
		//	this is where we can apply forces to things we hit
		PxShape*	shapes[1] = {};
		myActor->getShapes(shapes, 1);
		switch (shapes[0]->getGeometryType())
		{
		case PxGeometryType::eBOX:
		{
			myActor->addForce(-hit.worldNormal * myActor->getMass() * 100.0f);
			//std::cout << "Hit a BOX\n";
			break;
		}
		case PxGeometryType::eSPHERE:
		{
			myActor->addForce(hit.worldNormal * myActor->getMass() * 100.0f);
			//std::cout << "Hit a SPHERE\n";
			break;
		}
		default:
		{
			myActor->addForce(-hit.worldNormal * myActor->getMass() * 100.0f);
			break;
		}
		}
	}
	else
	{
		//	don't have to worry so much about this part - apparently the player can move the fluid nicely in Release, but the effect in Debug is very small!! :-/  ... fixed with a consistent dT.
		//PxParticleFluid*	myActor = actor->is<PxParticleFluid>();
		//if (myActor)
		//{
		//	//	this is where we can apply forces to things we hit
		//	//myActor->addForces();
		//}
	}
}
