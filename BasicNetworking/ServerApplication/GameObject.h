#pragma once

enum SyncType
{
	POSITION_ONLY,
	LERP,
	INTERPOLATION
};

struct GameObject
{
	unsigned int uiOwnerClientID;
	unsigned int uiObjectID;

	float fRedColour;
	float fGreenColour;
	float fBlueColour;

	float fXPos;
	float fZPos;

	float fXVel;
	float fZVel;

	SyncType	eSyncType;
};
