#include "Triggers.h"
#include <iostream>
using namespace std;


void	MyCollisionCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; ++i)
	{
		const PxContactPair&	cp = pairs[i];
		//	only interested in touches found and lost
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//	collision detected!
			PxShape*	shape0[1] = {};
			pairHeader.actors[0]->getShapes(shape0, 1);
			FilterGroup::Enum shape0Type;
			shape0Type = (FilterGroup::Enum)shape0[0]->getSimulationFilterData().word0;
			PxShape*	shape1[1] = {};
			pairHeader.actors[1]->getShapes(shape1, 1);
			FilterGroup::Enum shape1Type;
			shape1Type = (FilterGroup::Enum)shape1[0]->getSimulationFilterData().word0;
			if (((shape0Type == FilterGroup::eBOX) || (shape0Type == FilterGroup::ePLAYER) || (shape0Type == FilterGroup::eSPHERE))
				&& ((shape1Type == FilterGroup::eBOX) || (shape1Type == FilterGroup::ePLAYER) || (shape1Type == FilterGroup::eSPHERE)))
			{
				//cout << "Collision Detected between: " << pairHeader.actors[0]->getName()
				//	<< " and " << pairHeader.actors[1]->getName() << '\n';
			}
		}
	}
}

void	MyCollisionCallback::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	cout << "****---->>>>  Trigger has been hit!!\n";
//	for (PxU32 i = 0; i < nbPairs; ++i)
//	{
//		PxTriggerPair*	pair = pairs + i;
//		PxActor*	triggerActor = pair->triggerActor;
//		PxActor*	otherActor = pair->otherActor;
//		//triggerActor->userData;	//	wanted to have a reference to the SpawnRagdoll function, but haven't managed it so far ...
//		//cout << otherActor->getName() << " Entered Trigger of " << triggerActor->getName() << '\n';
//	}
}

//	WTF!? this only seems to work if it is declared *outside* a class ... :-/
PxFilterFlags	MyFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//	let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	//	generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	//	trigger the contact callback for pairs (A, B) where the filter mask of A contains the ID of B and vice versa
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
	}
	return PxFilterFlag::eDEFAULT;
}

//	helper function to set up filtering
void	SetupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup;	//	word0 = own ID
	filterData.word1 = filterMask;	//	word1 = ID mask to filter pairs that trigger a contact callback
	const PxU32	numShapes = actor->getNbShapes();
	PxShape**	shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*) * numShapes, 16);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; ++i)
	{
		PxShape*	shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}

void	SetShapeAsTrigger(PxRigidActor* actorIn)
{
	//	the tutorial asserted the shape was a static actor
	//	in my case I am leaving the shape to continue to take part in the physics simulation
	PxRigidStatic*	pActor = actorIn->is<PxRigidStatic>();
	if (pActor == nullptr)
	{
		cout << "Shape is not a static actor!\n";
		return;
	}
	//assert(pActor);
	const PxU32	numShapes = actorIn->getNbShapes();
	PxShape**	shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*) * numShapes, 16);
	actorIn->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; ++i)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	_aligned_free(shapes);
}
