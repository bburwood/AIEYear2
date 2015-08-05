#include "Ragdoll.h"


Ragdoll::Ragdoll()
{
}

Ragdoll::~Ragdoll()
{
}

PxArticulation*	Ragdoll::makeRagdoll(PxPhysics* g_Physics, RagDollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
{
	//	create the articulation for our ragdoll
	PxArticulation*	articulation = g_Physics->createArticulation();
	RagDollNode**	currentNode = nodeArray;
	//	while there are more nodes to process ...
	while (*currentNode != nullptr)
	{
		RagDollNode*	currentNodePtr = *currentNode;
		//	create a pointer ready to hold the parent node pointer if there is one
		RagDollNode*	parentNode = nullptr;
		//	get scaled values for capsule
		float	fRadius = currentNodePtr->radius * scaleFactor;
		float	fHalfLength = currentNodePtr->halfLength * scaleFactor;
		float	fChildHalfLength = fRadius + fHalfLength;
		float	fParentHalfLength = 0;	//	will be set later if there is a parent
		//	get a pointer to the parent
		PxArticulationLink*	parentLinkPtr = nullptr;
		currentNodePtr->scaledGlobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//	if there is a prent then we need to work out our local position for the link
			//	get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//	get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			fParentHalfLength = (parentNode->radius + parentNode->halfLength) * scaleFactor;
			//	work out the local position of the node
			PxVec3	currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(fChildHalfLength, 0, 0));
			PxVec3	parentRelative = -currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(fParentHalfLength, 0, 0));
			currentNodePtr->scaledGlobalPos = parentNode->scaledGlobalPos - (parentRelative + currentRelative);
		}
		//build the transform for the link
		PxTransform	linkTransform = PxTransform(currentNodePtr->scaledGlobalPos, currentNodePtr->globalRotation);
		//	create the link in the articulation
		PxArticulationLink*	link = articulation->createLink(parentLinkPtr, linkTransform);
		//	add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePtr->linkPtr = link;
		float	jointSpace = 0.01f;	//	gap between joints
		float	capsuleHalfLength = (fHalfLength > jointSpace ? fHalfLength - jointSpace : 0.0f) + 0.01f;
		PxCapsuleGeometry	capsule(fRadius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial);	//	adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);	//	adds some mass, mass should really be part of the data!

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//	finally set up the joint
			//	get the pointer to the joint from the link
			PxArticulationJoint*	joint = link->getInboundJoint();
			//	get the relative rotation of this link
			PxQuat	frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			//	set the parent constraint frame
			PxTransform	parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos * fParentHalfLength, 0, 0), frameRotation);
			//	set the child constraint frame (this is the constraint frame of the newly added link)
			PxTransform	thisConstraintFrame = PxTransform(PxVec3(currentNodePtr->childLinkPos * fChildHalfLength, 0, 0));
			//	set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//	set up some constraints to stop it flopping around
			joint->setStiffness(10);
			joint->setDamping(10);
			joint->setSwingLimit(1.f, 1.f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-1.f, 1.f);
			joint->setTwistLimitEnabled(true);
		}

		//	get a pointer to the current node
		currentNode++;
	}
	return articulation;
}

