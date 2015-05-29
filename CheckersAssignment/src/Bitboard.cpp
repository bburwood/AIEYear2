#include "Bitboard.h"

//	the coordinates of the game squares - need to have 2 arrays as the Kings are at a higher Y coordinate
vec3 aPieceCoords[32];
vec3 aKingCoords[32];
//	now the bitmasks
Bitboard	bbP1StartPieces = 0x00000FFF;	//	= 4,095
Bitboard	bbP1StartKings = 0;	//	start with no kings
Bitboard	bbP2StartPieces = 0xFFF00000;	//	= 4,293,918,720
Bitboard	bbP2StartKings = 0;	//	start with no kings

void	InitPieceCoordsArray()
{
	float	fPieceYCoord = 0.080f;
	float	fKingYCoord = 0.17f;
	float	fStartX = -3.5f;
	float	fStartZ = -3.5f;
	int		iOffsetRow = 1;	//	"top" row of the board is offset
	unsigned int uiIndex = 0;
	float	fXOffset;
	float	fZOffset = fStartZ;
	for (int z = 0; z < 8; ++z)
	{
		fXOffset = fStartX + (float)iOffsetRow;
		for (int x = 0; x < 4; ++x)
		{
			aPieceCoords[uiIndex] = vec3(fXOffset, fPieceYCoord, fZOffset);
			aKingCoords[uiIndex] = vec3(fXOffset, fKingYCoord, fZOffset);
			fXOffset += 2.0f;
			++uiIndex;
		}
		fZOffset += 1.0f;
		iOffsetRow = (iOffsetRow + 1) % 2;	//	makes every second row offset
	}
}

Bitboard	GenerateBitMaskFromCoords(int iX, int iZ)
{
	//	assumes that coordinates passed in are valid checkerboard coords
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	if ((iX == -1) || (iZ == -1))
	{
		//	then one of the coordinates is not on the board
		return bbResult;
	}
	int	iBitIndex = (iZ * 4) + (iX / 2);
	bbResult = 1 << iBitIndex;
	return bbResult;
}

