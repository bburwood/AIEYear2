#include "MyController.h"


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
		myActor->addForce(-hit.worldNormal * 1.0E3f);
	}
	else
	{
		//	don't have to worry so much about this part - apparently the player can move the fluid nicely in Release, but the effect in Debug is very small!! :-/
		//PxParticleFluid*	myActor = actor->is<PxParticleFluid>();
		//if (myActor)
		//{
		//	//	this is where we can apply forces to things we hit
		//	//myActor->addForces();
		//}
	}
}
