#ifndef _RAGDOLL_H_
#define _RAGDOLL_H_


#include <PxPhysicsAPI.h>
#include <PxScene.h>

using	namespace	physx;

const	PxVec3	X_AXIS = PxVec3(1, 0, 0);
const	PxVec3	Y_AXIS = PxVec3(0, 1, 0);
const	PxVec3	Z_AXIS = PxVec3(0, 0, 1);

//	Parts which make up our ragdoll
enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

struct RagDollNode
{
	PxQuat	globalRotation;	//	rotation of this link in model space - we could have done this relative to the parent node
							//	but it's harder to visualize when setting up the data by hand
	PxVec3	scaledGlobalPos;	//	Position of the link centre in world space which is calculated when we process the node.
								//	It's easiest if we store it here so we have it when we transform the child.
	int	parentNodeIdx;	//	Index of the parent node
	float	halfLength;	//	Half length of the capsule for this node
	float	radius;	//	radius of the capsule for this node
	float	parentLinkPos;	//	relative position of the link centre in parent to this node.
			//	0 is the centre of the node, -1 is the left end of capsule, and 1 is the right end of the capsule relative to x
	float	childLinkPos;	//	relative position of the link centre in child
	char*	name;	//	name of link
	PxArticulationLink*	linkPtr;
	//	constructor
	RagDollNode(PxQuat a_globalRotation, int a_parentNodeIdx, float a_halfLength, float a_radius, float a_parentLinkPos, float a_childLinkPos, char* a_name)
	{
		globalRotation = a_globalRotation;
		parentNodeIdx = a_parentNodeIdx;
		halfLength = a_halfLength;
		radius = a_radius;
		parentLinkPos = a_parentLinkPos;
		childLinkPos = a_childLinkPos;
		name = a_name;
	};

};

class Ragdoll
{
public:
	Ragdoll();
	~Ragdoll();
	static	PxArticulation*	makeRagdoll(PxPhysics* g_Physics, RagDollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial);

private:

};

#endif	//	_RAGDOLL_H_
