#ifndef _TRIGGERS_H_
#define _TRIGGERS_H_

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

PxFilterFlags	MyFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
void	SetupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
void	SetShapeAsTrigger(PxRigidActor* actorIn);

struct FilterGroup
{
	enum Enum
	{
		ePLAYER = (1 << 0),
		eBOX = (1 << 0),
		eSPHERE = (1 << 0),
		eRAGDOLL = (1 << 0),
		eCLOTH = (1 << 0),
		eGROUND = (1 << 0)
	};
};

class MyCollisionCallback : public PxSimulationEventCallback
{
public:
	virtual void	onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void	onTrigger(PxTriggerPair* pairs, PxU32 nbPairs);
	virtual void	onConstraintBreak(PxConstraintInfo*, PxU32){};
	virtual void	onWake(PxActor**, PxU32){};
	virtual void	onSleep(PxActor**, PxU32){};

private:

};

#endif	//	_TRIGGERS_H_
